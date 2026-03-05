#pragma once
#include "chunk.h"

#include <string>

namespace cpplox2 {

void disassemble_chunk(const Chunk& chunk, const std::string& name);
int disassemble_instruction(const Chunk& chunk, int offset);

} // namespace cpplox2
