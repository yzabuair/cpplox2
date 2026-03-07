#include "Vm.h"

#include "Debug.h"

#include <iostream>


namespace cpplox2 {

InterpretResult Vm::run() {
    if (curr_chunk_.data.empty()) {
        // TODO: Error..
    }
    
    for(;;) {
        int curr_ip = ip_++;
        uint8_t curr_ins = curr_chunk_.data[curr_ip];
        if (debug_) {
            std::cout << "stack = [";
            for(auto curr_val: stack_) {
                std::cout << curr_val << ", ";
            }
            std::cout << "]\n";
            disassemble_instruction(curr_chunk_, curr_ip);
        }
        switch(curr_ins) {
            case static_cast<uint8_t>(OpCode::OP_RETURN):
                std::cout << stack_.front() << "\n";
                stack_.pop_front();
                return InterpretResult::INTERPRET_OK;
                
            case static_cast<uint8_t>(OpCode::OP_ADD):
                binary_op_(OpCode::OP_ADD);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_SUBTRACT):
                binary_op_(OpCode::OP_SUBTRACT);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_MULTIPLY):
                binary_op_(OpCode::OP_MULTIPLY);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_DIVIDE):
                binary_op_(OpCode::OP_DIVIDE);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_NEGATE):
                {
                    auto val = stack_.front();
                    stack_.pop_front();
                    stack_.push_front(-1 * val);
                }
                break;
                
            case static_cast<uint8_t>(OpCode::OP_CONSTANT):
                uint8_t const_index = curr_chunk_.data[ip_++];
                stack_.push_front(curr_chunk_.constants.constants[const_index]);
                break;
        }
    }
    
    return InterpretResult::INTERPRET_OK;
}

// Internal Helpers

void Vm::binary_op_(OpCode which_op) {
    auto b = stack_.front();
    stack_.pop_front();
    auto a = stack_.front();
    stack_.pop_front();
    
    switch (which_op) {
        case OpCode::OP_ADD:
            stack_.push_front(a + b);
            break;
        case OpCode::OP_SUBTRACT:
            stack_.push_front(a - b);
            break;
        case OpCode::OP_MULTIPLY:
            stack_.push_front(a * b);
            break;
        case OpCode::OP_DIVIDE:
            stack_.push_front(a / b);
            break;
        default:
            // TODO: Error.
            break;
    }
}

} // namespace cpplox2
