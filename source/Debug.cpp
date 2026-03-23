// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#include "Debug.h"

#include <iostream>
#include <iomanip>
#include <string>

namespace cpplox2 {

namespace {
int simple_instruction_(const std::string& name, int offset) {
    std::cout << name << "\n";
    return offset + 1;
}

int const_instruction_(const std::string& name, const Chunk& chunk, int offset) {
    uint8_t constant_id = chunk.data[offset + 1];
    std::cout << std::setfill(' ') << std::setw(16) << std::left << name;
    std::cout << std::setw(4) << std::to_string(constant_id) << std::get<double>(chunk.constants.constants[constant_id]) << "\n";
    return offset + 2;
}

} // namespace

void disassemble_chunk(const Chunk& chunk, const std::string& name) {
    std::cout << "=== " << name << " === \n";
    for(int i = 0; i < chunk.data.size();) {
        i = disassemble_instruction(chunk, i);
    }
}

int disassemble_instruction(const Chunk& chunk, int offset) {
    std::cout << "[" << std::setfill('0') << std::setw(4) << std::right << offset << "] ";
    
    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset-1]) {
        std::cout << "   | ";
    } else {
        std::cout << std::setw(4) << std::setfill(' ') << chunk.lines[offset] << " ";
    }
    
    uint8_t instruction = chunk.data[offset];
    switch (instruction) {
        case static_cast<int>(OpCode::OP_RETURN):
            return simple_instruction_("OP_RETURN", offset);

        case static_cast<uint8_t>(OpCode::OP_ADD):
            return simple_instruction_("OP_ADD", offset);
            
        case static_cast<uint8_t>(OpCode::OP_SUBTRACT):
            return simple_instruction_("OP_SUBTRACT", offset);
            
        case static_cast<uint8_t>(OpCode::OP_MULTIPLY):
            return simple_instruction_("OP_MULTIPLY", offset);
            
        case static_cast<uint8_t>(OpCode::OP_DIVIDE):
            return simple_instruction_("OP_DIVIDE", offset);
            
        case static_cast<int>(OpCode::OP_NEGATE):
            return simple_instruction_("OP_NEGATE", offset);
            
        case static_cast<int>(OpCode::OP_CONSTANT):
            return const_instruction_("OP_CONSTANT", chunk, offset);
            
        case static_cast<int>(OpCode::OP_NIL):
            return simple_instruction_("OP_NIL", offset);
            
        case static_cast<int>(OpCode::OP_TRUE):
            return simple_instruction_("OP_TRUE", offset);
            
        case static_cast<int>(OpCode::OP_FALSE):
            return simple_instruction_("OP_FALSE", offset);
        
        case static_cast<int>(OpCode::OP_NOT):
            return simple_instruction_("OP_NOT", offset);
            
        case static_cast<int>(OpCode::OP_GREATER):
            return simple_instruction_("OP_GREATER", offset);
            
        case static_cast<int>(OpCode::OP_LESS):
            return simple_instruction_("OP_LESS", offset);
            
        case static_cast<int>(OpCode::OP_EQUAL):
            return simple_instruction_("OP_EQUAL", offset);

        default:
            std::cout << "Unkown code: " << static_cast<int>(instruction) << "\n";
            return offset + 1;
    }
}


} // namespace cpplox2
