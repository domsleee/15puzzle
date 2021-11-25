#include "../include/IdastarMulti.h"

#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/Util.h"
#include "../include/Idastar.h"
#include <fcntl.h> // library for fcntl function


#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <numeric>
#include <sys/types.h>
#include <signal.h>

#include <chrono>
#include <thread>

#define DEBUG_MULTI(x)

#define DEBUG_WITH_PID(x) DEBUG_MULTI(getpid() << ": " << x)
#define DEBUG_HOST(x) DEBUG_MULTI("HOST: " << x)

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

    auto initialNodeGetter = IdastarMultiInitialNodes<B>(fsm);

    nodes = 1;
    limit = start.getHeuristic();

    if (limit == 0) {
        DEBUG("Already solved");
        return path;
    }

    DEBUG("get initial nodes");
    START_TIMER(INITIAL_NODES);
    auto targetWorkers = 63;
    auto initialNodes = initialNodeGetter.getInitialNodes2(start, targetWorkers);
    END_TIMER(INITIAL_NODES);

    DEBUG("num initialNodes: " << initialNodes.size());
    if (initialNodes.size() == 0) {
        exit(2);
    }

    numWorkers = std::min(targetWorkers, (int)initialNodes.size());
    serverReadPipes.assign(numWorkers, {0, 0});
    serverWritePipes.assign(numWorkers, {0, 0});

    std::vector<int> pids;

    for (auto i = 0; i < numWorkers; ++i) {
        pipe(serverReadPipes[i].data());
        pipe(serverWritePipes[i].data());
    }
    // set all to non-blocking
    for (auto i = 0; i < numWorkers; ++i) {
        if (fcntl(serverReadPipes[i][0], F_SETFL, O_NONBLOCK) < 0) {
            DEBUG_HOST("failed to set non blocking for pipe");
            exit(2);
        }
    }
    for (auto i = 0; i < numWorkers; ++i) {
        pid_t cpid = fork();
        if (cpid == 0) {
            doClient(i, initialNodes);
        }
        pids.push_back(cpid);
        // DEBUG("serverReadPipes[" << i << "] = [" << serverReadPipes[i][0] << " , " << serverReadPipes[i][1] << "]");
        // DEBUG("serverWritePipes[" << i << "] = [" << serverWritePipes[i][0] << " , " << serverWritePipes[i][1] << "]");
    }

    // server
    DEBUG("Limit, Nodes:");

    std::vector<std::vector<int>> initialNodeAllocations(numWorkers, std::vector<int>());
    for (auto i = 0; i < numWorkers; ++i) {
        for (auto j = i; j < initialNodes.size(); j += numWorkers) {
            initialNodeAllocations[i].push_back(j);
        }
    }

    for (auto i = 0; i < numWorkers; ++i) {
        auto allocation = initialNodeAllocations[i];
        auto allocationSize = allocation.size();

        DEBUG_WITH_PID("worker " << i << " has " << allocationSize);
        auto serverWritePipe = serverWritePipes[i];
        write(serverWritePipe[1], &allocationSize, sizeof(int));
        for (auto nodeId: allocation) {
            write(serverWritePipe[1], &nodeId, sizeof(int));
        }
    }

    while (path.empty()) {
        minCost = INF;
        DEBUG(' ' << limit << ", " << nodes);
        writeAll(&COMMAND_PROCESS, sizeof(COMMAND_PROCESS));
        writeAll(&limit, sizeof(limit));

        long long outNodes[numWorkers];
        int result, outMinCost[numWorkers];
        bool isFound = false;

        int msg = 0;
        while (true) {
            DEBUG_HOST("received " << msg << "/" << numWorkers << " " << (100 * msg) / numWorkers << "%");
            for (auto i = 0; i < numWorkers; ++i) {
                if (initialNodeAllocations[i].size() == 0) continue;

                auto serverReadPipe = serverReadPipes[i];
                if (read(serverReadPipe[0], &result, sizeof(result)) == -1) continue;
                //if (fcntl(serverReadPipe[0], F_SETFL, fcntl(serverReadPipe[0], F_GETFL) & (~O_NONBLOCK)) < 0) exit(2);

                if (result == RESULT_FOUND) {
                    int dfsPathSize, nodeId;
                    while (read(serverReadPipe[0], &nodeId, sizeof(nodeId)) == -1);
                    while (read(serverReadPipe[0], &dfsPathSize, sizeof(dfsPathSize)) == -1);
                    DEBUG_HOST("FOUND WITH NODE " << nodeId << " DFS SIZE " << dfsPathSize);
                    auto initialPathSize = initialNodes[nodeId].path.size();
                    path.resize(dfsPathSize + initialPathSize);

                    for (auto j = 0; j < dfsPathSize; ++j) {
                        while (read(serverReadPipe[0], &path[0+j], sizeof(Direction)) == -1);
                        //DEBUG("PATH " << path[0+j]);
                    }

                    for (auto j = 0; j < initialPathSize; ++j) {
                        path[dfsPathSize+j] = initialNodes[nodeId].path[j];
                    }


                    isFound = true;
                    break;
                } else if (result == RESULT_NOFIND) {
                    while (read(serverReadPipe[0], &outNodes[i], sizeof(outNodes[i])) == -1);
                    while (read(serverReadPipe[0], &outMinCost[i], sizeof(outMinCost[i])) == -1);
                    
                    msg++;
                    DEBUG_HOST("output " << outNodes[i] << " " << outMinCost[i]);
                } else {
                    assertm("unrecognised result.", 0);
                }
            }

            if (isFound || msg == numWorkers) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        if (isFound) break;

        nodes = 1 + std::accumulate(outNodes, outNodes + numWorkers, 0);
        limit = *std::min_element(outMinCost, outMinCost + numWorkers);
    }
    writeAll(&COMMAND_FINISH, sizeof(COMMAND_FINISH));

    for (auto cpid: pids) {
        kill(cpid, SIGKILL);
    }

    for (auto cpid: pids) {
        int status;
        DEBUG_MULTI("WAITING FOR CHILD " << cpid);
        while (waitpid(cpid, &status, 0) != cpid) {
            //DEBUG(cpid << " status " << status);
            if (status == 10) {
                //DEBUG("child " << cpid << " warning??");
                //break;
            } else {
                DEBUG_MULTI(" status" << status);
            }
        }
        DEBUG_MULTI("Child " << cpid << " returned");
    }

    for (auto pipe: serverWritePipes) {
        close(pipe[0]);
        close(pipe[1]);
    }
    for (auto pipe: serverReadPipes) {
        close(pipe[0]);
        close(pipe[1]);
    }

    return path;
}

template <class B>
void IdastarMulti<B>::writeAll(const void* ptr, size_t size) {
    for (auto pipe: serverWritePipes) {
        write(pipe[1], ptr, size);
    }
}

template <class B>
void IdastarMulti<B>::doClient(int nodeId, std::vector<typename IdastarMultiInitialNodes<B>::InitialNode> initialNodes) {
    auto serverWritePipe = serverWritePipes[nodeId];
    auto serverReadPipe = serverReadPipes[nodeId];

    int numNodesToProcess;
    read(serverWritePipe[0], &numNodesToProcess, sizeof(int));
    DEBUG_WITH_PID("numNodesToProcess: " << numNodesToProcess);
    if (numNodesToProcess == 0) {
        DEBUG_WITH_PID("EARLY EXIT");
        exit(0);
        return;
    }
    std::vector<int> nodesToProcess(numNodesToProcess, -1);
    for (int i = 0; i < numNodesToProcess; ++i) read(serverWritePipe[0], &nodesToProcess[i], sizeof(int));

    DEBUG_WITH_PID("read all nodestoprocess");
    Idastar<B> idastar(fsm);

    int command, limit = 0;
    while (true) { // read until EOF
        read(serverWritePipe[0], &command, sizeof(int));
        if (command == COMMAND_FINISH) {
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

        for (auto nodeId: nodesToProcess) {
            idastar.clearPathAndSetLimit(limit);

            auto initialNode = initialNodes[nodeId];
            auto startBoard = initialNode.startBoard;
            DEBUG_WITH_PID("processing node " << nodeId << " " << initialNode.fsmState);
            fsm.undoMove(initialNode.fsmState);

            if (idastar.dfs(startBoard, initialNode.g)) {
                DEBUG_WITH_PID("FOUND PATH " << idastar.path.size());
                write(serverReadPipe[1], &RESULT_FOUND, sizeof(int));
                write(serverReadPipe[1], &nodeId, sizeof(nodeId));

                int pathSize = idastar.path.size();
                write(serverReadPipe[1], &pathSize, sizeof(pathSize));
                for (auto dir: idastar.path) {
                    //DEBUG_WITH_PID("path " << dir);
                    write(serverReadPipe[1], &dir, sizeof(dir));
                }
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

    DEBUG_WITH_PID("FINISHED");
    exit(0);
}

template class IdastarMulti<Board>;
template class IdastarMulti<BoardRect>;
