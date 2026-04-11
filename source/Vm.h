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

/// The Virtual Machine that executes the instructions in a chunk.  This is stack-based.
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
    Value result();
    
// Internal Helpers
private:
    void binary_op_(OpCode which_op);
    bool is_falsey_(const Value& value);
    bool values_equal_(const Value& value1, const Value& value2);
    
    
};

} // namespace cpplox2
