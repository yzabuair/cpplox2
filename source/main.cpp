#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, char* argv[]) {
    cpplox2::Chunk chunk;
    
    int offset = chunk.constants.add_value(1.2);
    chunk.write(cpplox2::OpCode::OP_CONSTANT);
    chunk.write(offset);
    
    chunk.write(cpplox2::OpCode::OP_RETURN);
    
    cpplox2::disassemble_chunk(chunk, "test-chunk");
    
    return 0;
}
