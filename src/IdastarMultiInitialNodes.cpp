
#include "../include/IdastarMultiInitialNodes.h"
#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/Util.h"

template <class B>
IdastarMultiInitialNodes<B>::InitialNode::InitialNode(B startBoard, int g, int fsmState, const std::vector<Direction> &path)
    : g(g),
      fsmState(fsmState),
      startBoard(startBoard),
      path(path) {}

template <class B>
IdastarMultiInitialNodes<B>::IdastarMultiInitialNodes(StateMachine &fsm)
    : fsm(fsm)
    {}

template <class B>
std::vector<typename IdastarMultiInitialNodes<B>::InitialNode> IdastarMultiInitialNodes<B>::getInitialNodes(const B& start) {
    std::vector<InitialNode> res;

    fsm.undoMove(0);
    for (int i = 0; i < 4; ++i) {
        auto node = start;
        auto dir = static_cast<Direction>(i);
        if (fsm.canMove(i) && node.canMove(dir)) {
            auto prev = node.applyMove(dir);
            auto prevFsm = fsm.applyMove(i);

            DEBUG("INITIAL DIR " << dir << " i " << i << " INDEX " << res.size() << " FSM " << fsm.state);
            res.push_back(InitialNode(node, 1, fsm.state, {dir}));

            fsm.undoMove(prevFsm);
            node.undoMove(prev);
        }
    }

    return res;
}

template class IdastarMultiInitialNodes<Board>;
template class IdastarMultiInitialNodes<BoardRect>;
