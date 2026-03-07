// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once
#include "Chunk.h"

#include <deque>

namespace cpplox2 {

enum class InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class Vm {
    Chunk curr_chunk_;
    bool debug_{false};
    int ip_{0};
    std::deque<Value> stack_;

    
public:
    Vm(const Chunk& chunk,
       bool debug):
        curr_chunk_{chunk},
        debug_{debug} {
    }
    
    InterpretResult run();
    
// Internal Helpers
private:
    void binary_op_(OpCode which_op);
    
    
};

} // namespace cpplox2
