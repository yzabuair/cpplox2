// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once
#include "Value.h"

#include <memory>
#include <vector>

namespace cpplox2 {

enum class OpCode {
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_NEGATE,
    OP_RETURN,
};

struct Chunk {
    std::vector<uint8_t> data;
    std::vector<int> lines;
    Values constants;
    
    Chunk& write(OpCode op_code, int line) {
        data.push_back(static_cast<uint8_t>(op_code));
        lines.push_back(line);
        return *this;
    }
    
    Chunk& write(uint8_t byte, int line) {
        data.push_back(static_cast<uint8_t>(byte));
        lines.push_back(line);
        return *this;
    }
    
    void clear() {
        data.clear();
        lines.clear();
        constants.clear();
    }
};

} // namespace cpplox2
