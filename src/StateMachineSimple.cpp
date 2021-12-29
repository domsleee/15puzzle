#include "../include/StateMachineSimple.h"

StateMachineSimple::StateMachineSimple(const std::vector<std::array<int, 4>> &&g)
    : g(g),
      state(0)
    {}
