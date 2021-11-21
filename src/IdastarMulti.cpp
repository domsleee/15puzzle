#include "../include/IdastarMulti.h"

#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/Util.h"
#include "../include/Idastar.h"

#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <numeric>

constexpr int INF = 1000;
constexpr int COMMAND_PROCESS = 43;
constexpr int COMMAND_FINISH = 44;


constexpr int RESULT_NOFIND = 55;
constexpr int RESULT_FOUND = 56;

template <class B>
IdastarMulti<B>::IdastarMulti(StateMachine &fsm) : fsm(fsm), nodes(0), limit(0), minCost(0) {}

template <class B>
std::vector<Direction> IdastarMulti<B>::solve(const B& start) {
    std::vector<Direction> path;
    
    nodes = 1;
    limit = start.getHeuristic();
    width = start.WIDTH;
    height = start.HEIGHT;

    if (limit == 0) {
        DEBUG("Already solved");
        return path;
    }

    DEBUG("Limit, Nodes:");

    numWorkers = 1;
    serverReadPipes.assign(numWorkers, {0, 0});
    serverWritePipes.assign(numWorkers, {0, 0});

    std::vector<int> pids;

    auto initialNodes = getInitialNodes(start);

    for (auto i = 0; i < numWorkers; ++i) {
        pipe(serverReadPipes[i].data());
        pipe(serverWritePipes[i].data());
        pid_t cpid = fork();
        if (cpid == 0) {
            doClient(initialNodes);
        }
        pids.push_back(cpid);
    }

    // server
    auto initialNodesSize = initialNodes.size();
    writeAll(&initialNodesSize, sizeof(int));
    for (auto i = 0; i < initialNodes.size(); ++i) {
        writeAll(&i, sizeof(int));
    }

    while (path.empty()) {
        minCost = INF;
        DEBUG(' ' << limit << ", " << nodes);
        writeAll(&COMMAND_PROCESS, sizeof(COMMAND_PROCESS));
        writeAll(&limit, sizeof(limit));

        long long outNodes[numWorkers];
        int result, outMinCost[numWorkers];
        bool isFound = false;

        for (auto i = 0; i < numWorkers; ++i) {
            auto serverReadPipe = serverReadPipes[i];
            read(serverReadPipe[0], &result, sizeof(result));
            if (result == RESULT_FOUND) {
                int pathSize;
                read(serverReadPipe[0], &pathSize, sizeof(pathSize));
                DEBUG("FOUND WITH SIZE " << pathSize);
                isFound = true;
                break;
            } else if (result == RESULT_NOFIND) {

                read(serverReadPipe[0], &outNodes[i], sizeof(outNodes[i]));
                read(serverReadPipe[0], &outMinCost[i], sizeof(outMinCost[i]));
                
                DEBUG("output " << outNodes[i] << " " << outMinCost[i]);
            } else {
                assertm("unrecognised result.", 0);
            }
        }

        if (isFound) break;

        nodes = 1 + std::accumulate(outNodes, outNodes + numWorkers, 0);
        limit = *std::min_element(outMinCost, outMinCost + numWorkers);
    }
    writeAll(&COMMAND_FINISH, sizeof(COMMAND_FINISH));

    for (auto cpid: pids) {
        int status;
        DEBUG("WAITING FOR CHILD " << cpid);
        while (wait(&status) != cpid) {
            DEBUG(cpid << " status " << status);
        }
        DEBUG("Child " << cpid << " returned");
    }

    for (auto pipe: serverWritePipes) {
        close(pipe[0]);
        close(pipe[1]);
    }
    for (auto pipe: serverReadPipes) {
        close(pipe[0]);
        close(pipe[1]);
    }
    return {};
}

template <class B>
void IdastarMulti<B>::writeAll(const void* ptr, size_t size) {
    for (auto pipe: serverWritePipes) {
        write(pipe[1], ptr, size);
    }
}

template <class B>
void IdastarMulti<B>::doClient(std::vector<typename IdastarMulti<B>::InitialNode> initialNodes) {
    auto serverWritePipe = serverWritePipes[0];
    auto serverReadPipe = serverReadPipes[0];

    std::vector<B> initialBoards;
    for (auto initialNode: initialNodes) {
        DEBUG("PRINT GRID");
        for (auto c: initialNode.grid) DEBUG(c);
        DEBUG("SDLFJ");
        initialBoards.push_back(B(initialNode.grid, width, height));
    }

    int numNodesToProcess;
    read(serverWritePipe[0], &numNodesToProcess, sizeof(int));
    DEBUG_WITH_PID("numNodesToProcess: " << numNodesToProcess);
    std::vector<int> nodesToProcess(numNodesToProcess, -1);
    for (int i = 0; i < numNodesToProcess; ++i) read(serverWritePipe[0], &nodesToProcess[i], sizeof(int));

    DEBUG_WITH_PID("read all nodestoprocess");
    auto fsmClone = fsm;
    Idastar<B> idastar(fsmClone);
    fsm.undoMove(0);

    int command, limit = 0;
    while (true) { // read until EOF
        read(serverWritePipe[0], &command, sizeof(int));
        if (command == COMMAND_FINISH) {
            DEBUG_WITH_PID("finish!");
            break;
        } else if (command == COMMAND_PROCESS) {
            // all good
        } else {
            DEBUG_WITH_PID("unrecognised command " << command);
            break;
        }

        read(serverWritePipe[0], &limit, sizeof(int));

        // process
        DEBUG_WITH_PID("read limit as " << limit);
        idastar.clearPathAndSetLimit(limit);

        for (auto nodeId: nodesToProcess) {
            auto startBoard = initialBoards[nodeId];
            auto initialNode = initialNodes[nodeId];
            DEBUG_WITH_PID("processing node " << nodeId << " " << initialNode.fsmState);
            fsm.undoMove(initialNode.fsmState);
            if (idastar.dfs(startBoard, initialNode.g)) {
                DEBUG_WITH_PID("FOUND PATH " << idastar.path.size());
                write(serverReadPipe[1], &RESULT_FOUND, sizeof(int));
                auto pathSize = idastar.path.size();
                write(serverReadPipe[1], &pathSize, sizeof(pathSize));
                break;
            } else {
                DEBUG_WITH_PID("dfs return false");
            }
        }

        auto nodes = idastar.getNodes();
        auto minCost = idastar.getMinCost();
        write(serverReadPipe[1], &RESULT_NOFIND, sizeof(int));
        write(serverReadPipe[1], &nodes, sizeof(nodes));
        write(serverReadPipe[1], &minCost, sizeof(minCost));
    }
    //close(serverReadPipe[0]);
   // close(serverReadPipe[1]);
    DEBUG_WITH_PID("FINISHED?");
    exit(0);
}

template <class B>
IdastarMulti<B>::InitialNode::InitialNode(B startNode, int g, int fsmState)
    : g(g),
      fsmState(fsmState) {
        grid = startNode.getGrid();
        DEBUG("MCSIZE: " << grid.size());
        for (auto c: grid) DEBUG(c);
      }

template <class B>
std::vector<typename IdastarMulti<B>::InitialNode> IdastarMulti<B>::getInitialNodes(const B& start) {
    std::vector<InitialNode> res;

    for (int i = 0; i < 4; ++i) {
        auto node = start;
        auto dir = static_cast<Direction>(i);
        if (fsm.canMove(i) && node.canMove(dir)) {
            auto prev = node.applyMove(dir);
            auto prevFsm = fsm.applyMove(i);
            
            res.push_back(InitialNode(node, 1, fsm.state));

            fsm.undoMove(prevFsm);
            node.undoMove(prev);
        }
    }

    return res;
}

template class IdastarMulti<Board>;
template class IdastarMulti<BoardRect>;
