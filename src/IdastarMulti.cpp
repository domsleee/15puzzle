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
#include <sys/wait.h>

#include <chrono>
#include <thread>

#define DEBUG_MULTI(x)

#define DEBUG_WITH_PID(x) DEBUG_MULTI(getpid() << ": " << x)
#define DEBUG_HOST(x) DEBUG_MULTI("HOST: " << x)

constexpr int INF = 1000;

constexpr int RESULT_NOFIND = 55;
constexpr int RESULT_FOUND = 56;

template <class B>
IdastarMulti<B>::IdastarMulti(StateMachineSimple &fsm) : fsm(fsm), limit(0), minCost(0), nodes(0) {}

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
    auto targetWorkers = 62; // weird?
    auto initialNodes = initialNodeGetter.getInitialNodes2(start, 50 * targetWorkers);
    END_TIMER(INITIAL_NODES);

    DEBUG("num initialNodes: " << initialNodes.size());
    if (initialNodes.size() == 0) {
        exit(2);
    }

    numWorkers = std::min(targetWorkers, (int)initialNodes.size());
    serverReadPipes.assign(numWorkers, {0, 0});
    serverWritePipes.assign(numWorkers, {0, 0});
    sharedPipe.assign({0, 0});

    std::vector<int> pids;

    for (auto i = 0; i < numWorkers; ++i) {
        pipe(serverReadPipes[i].data());
        pipe(serverWritePipes[i].data());
    }
    pipe(sharedPipe.data());

    for (auto i = 0; i < numWorkers; ++i) {
        pid_t cpid = fork();
        if (cpid == 0) {
            doClient(i, initialNodes);
            exit(0);
        }
        pids.push_back(cpid);
    }

    // server
    DEBUG("Limit, Nodes:");

    std::vector<std::vector<int>> initialNodeAllocations(numWorkers, std::vector<int>());
    for (auto i = 0; i < numWorkers; ++i) {
        for (auto j = i; j < static_cast<int>(initialNodes.size()); j += numWorkers) {
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
        writeAll(&limit, sizeof(limit));

        long long outNodes[numWorkers];
        int result, outMinCost[numWorkers];
        bool isFound = false;

        int msg = 0;
        while (true) {
            DEBUG_HOST("received " << msg << "/" << numWorkers << " " << (100 * msg) / numWorkers << "%");
            int workerId;
            read(sharedPipe[0], &workerId, sizeof(int));
            DEBUG_HOST("received from shared pipe " << workerId);
            auto serverReadPipe = serverReadPipes[workerId];
            read(serverReadPipe[0], &result, sizeof(result));

            if (result == RESULT_FOUND) {
                int dfsPathSize, nodeId;
                read(serverReadPipe[0], &nodeId, sizeof(nodeId));
                read(serverReadPipe[0], &dfsPathSize, sizeof(dfsPathSize));
                DEBUG_HOST("FOUND WITH NODE " << nodeId << " DFS SIZE " << dfsPathSize);
                int initialPathSize = initialNodes[nodeId].path.size();
                path.resize(dfsPathSize + initialPathSize);

                for (auto j = 0; j < dfsPathSize; ++j) {
                    read(serverReadPipe[0], &path[0+j], sizeof(Direction));
                    //DEBUG("PATH " << path[0+j]);
                }

                for (auto j = 0; j < initialPathSize; ++j) {
                    path[dfsPathSize+j] = initialNodes[nodeId].path[j];
                }

                isFound = true;
                break;
            } else if (result == RESULT_NOFIND) {
                read(serverReadPipe[0], &outNodes[workerId], sizeof(outNodes[workerId]));
                read(serverReadPipe[0], &outMinCost[workerId], sizeof(outMinCost[workerId]));
                
                msg++;
                DEBUG_HOST("output " << outNodes[workerId] << " " << outMinCost[workerId]);
            } else {
                assertm("unrecognised result.", 0);
            }

            if (isFound || msg == numWorkers) break;
        }

        if (isFound) break;

        nodes = 1 + std::accumulate(outNodes, outNodes + numWorkers, 0LL);
        limit = *std::min_element(outMinCost, outMinCost + numWorkers);
    }
    //writeAll(&COMMAND_FINISH, sizeof(COMMAND_FINISH));

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
    close(sharedPipe[0]);
    close(sharedPipe[1]);

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
    int workerId = nodeId;
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

    int limit = 0;
    while (true) { // read until EOF
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
                write(sharedPipe[1], &workerId, sizeof(workerId));
                write(serverReadPipe[1], &RESULT_FOUND, sizeof(int));
                write(serverReadPipe[1], &nodeId, sizeof(nodeId));

                int pathSize = idastar.path.size();
                write(serverReadPipe[1], &pathSize, sizeof(pathSize));
                for (auto dir: idastar.path) {
                    //DEBUG_WITH_PID("path " << dir);
                    write(serverReadPipe[1], &dir, sizeof(dir));
                }
                exit(0);
            } else {
                DEBUG_WITH_PID("dfs return false");
            }
        }

        auto nodes = idastar.getNodes();
        auto minCost = idastar.getMinCost();
        write(sharedPipe[1], &workerId, sizeof(workerId));
        write(serverReadPipe[1], &RESULT_NOFIND, sizeof(int));
        write(serverReadPipe[1], &nodes, sizeof(nodes));
        write(serverReadPipe[1], &minCost, sizeof(minCost));
    }

    DEBUG_WITH_PID("FINISHED");
    exit(0);
}

template class IdastarMulti<Board>;
template class IdastarMulti<BoardRect>;
