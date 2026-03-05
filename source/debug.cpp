#include "debug.h"

#include <iostream>
#include <iomanip>

namespace cpplox2 {

namespace {
int simple_instruction_(const std::string& name, int offset) {
    std::cout << name << "\n";
    return offset + 1;
}

int const_instruction_(const std::string& name, const Chunk& chunk, int offset) {
    uint8_t constant = chunk.data[offset + 1];
    std::cout << std::setfill(' ') << std::setw(16) << std::left << name  << 1.25 << "\n";
    return offset + 2;
}

} // namespace

void disassemble_chunk(const Chunk& chunk, const std::string& name) {
    std::cout << "=== " << name << " === \n";
    for(int i = 0; i < chunk.data.size(); ++i) {
        disassemble_instruction(chunk, i);
    }
}

int disassemble_instruction(const Chunk& chunk, int offset) {
    std::cout << "[" << std::setfill('0') << std::setw(4) << std::right << offset << "] ";
    
    uint8_t instruction = chunk.data[offset];
    switch (instruction) {
        case static_cast<int>(OpCode::OP_RETURN):
            return simple_instruction_("OP_RETURN", offset);
            break;
        case static_cast<int>(OpCode::OP_CONSTANT):
            return const_instruction_("OP_CONSTANT", chunk, offset);
            break;
        default:
            std::cout << "Unkown code: " << instruction << "\n";
            return offset + 1;
            break;
    }
}


} // namespace cpplox2
