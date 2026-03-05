#pragma once
#include "value.h"

#include <memory>
#include <vector>

namespace cpplox2 {

enum class OpCode {
    OP_CONSTANT,
    OP_RETURN,
};

struct Chunk {
    std::vector<uint8_t> data;
    Values constants;
    
    Chunk& write(OpCode op_code) {
        data.push_back(static_cast<uint8_t>(op_code));
        return *this;
    }
    
    Chunk& write(int offset) {
        data.push_back(static_cast<uint8_t>(offset));
        return *this;
    }
};

} // namespace cpplox2
