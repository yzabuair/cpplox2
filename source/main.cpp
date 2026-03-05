#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "cpplox2, (c) 2026 Yasser Zabuair\n";
    std::cout << "version 0.00.01\n";
    std::cout << "See LICENSE for details.\n";
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n";
    std::cout << "---\n";
    
    cpplox2::Chunk chunk;
    
    int constant_index = chunk.constants.add_value(1.2);
    chunk.write(cpplox2::OpCode::OP_CONSTANT, 123)
         .write(constant_index, 123);
    
    constant_index = chunk.constants.add_value(3.4);
    chunk.write(cpplox2::OpCode::OP_CONSTANT, 123)
         .write(constant_index, 123);
    
    chunk.write(cpplox2::OpCode::OP_ADD, 123);
    
    constant_index = chunk.constants.add_value(5.6);
    chunk.write(cpplox2::OpCode::OP_CONSTANT, 123)
         .write(constant_index, 123);
    
    chunk.write(cpplox2::OpCode::OP_DIVIDE, 123);
    
    chunk.write(cpplox2::OpCode::OP_NEGATE, 123);
    
    chunk.write(cpplox2::OpCode::OP_RETURN, 123);
    
    //cpplox2::disassemble_chunk(chunk, "test-chunk");
    cpplox2::Vm vm{chunk, true};
    vm.run();
    
    return 0;
}
