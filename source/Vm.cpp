// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#include "Vm.h"

#include "Debug.h"
#include "RuntimeError.h"
#include "Value.h"

#include <iostream>


namespace cpplox2 {

InterpretResult Vm::run() {
    if (curr_chunk_.data.empty()) {
        // TODO: Error..
    }
    
    for(;;) {
        int curr_ip = ip_++;
        if (curr_ip >= curr_chunk_.data.size()) {
            // We are done, son.
            break;
        }
        
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
            case static_cast<uint8_t>(OpCode::OP_PRINT):
                {
                    auto value = stack_.front();
                    stack_.pop_front();
                    std::cout << value << "\n";
                }
                break;
                
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
                
            case static_cast<uint8_t>(OpCode::OP_NOT):
                {
                    auto val = stack_.front();
                    stack_.pop_front();
                    stack_.push_front(is_falsey_(val));
                }
                break;
            case static_cast<uint8_t>(OpCode::OP_NEGATE):
                {
                    // We can only negate a numeric type.
                    if (stack_.front().index() != 2) {
                        throw RuntimeError("You only negate a number.");
                    }
                    auto val = std::get<double>(stack_.front());
                    stack_.pop_front();
                    stack_.push_front(-1 * val);
                }
                break;
                
            case static_cast<uint8_t>(OpCode::OP_CONSTANT):
                {
                    uint8_t const_index = curr_chunk_.data[ip_++];
                    stack_.push_front(curr_chunk_.constants.constants[const_index]);
                }
                break;
                
            case static_cast<uint8_t>(OpCode::OP_NIL):
                stack_.push_front(Value{});
                break;
                
            case static_cast<uint8_t>(OpCode::OP_TRUE):
                stack_.push_front(true);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_FALSE):
                stack_.push_front(false);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_POP):
                stack_.pop_front();
                break;
                
            case static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL):
                {
                    // Get the index of the variable and its name.
                    uint8_t const_index = curr_chunk_.data[ip_++];
                    std::string var_name = std::get<std::string>(curr_chunk_.constants.get_value(const_index));
                    
                    // Get the initial value that's on the stack.
                    Value init_value = stack_.front();
                    
                    globals_.set_symbol(var_name, init_value);
                    
                    stack_.pop_front();
                }
                break;
                
            case static_cast<uint8_t>(OpCode::OP_GET_GLOBAL):
                {
                    // Get the index of the variable and its name.
                    uint8_t const_index = curr_chunk_.data[ip_++];
                    std::string var_name = std::get<std::string>(curr_chunk_.constants.get_value(const_index));
                    
                    Value value;
                    bool result = globals_.get_symbol(var_name, value);
                    if (!result) {
                        std::stringstream stream;
                        stream << "Undefined global variable: " << var_name;
                        throw RuntimeError(stream.str());
                    }
                    stack_.push_front(value);
                    
                }
                break;
                
            case static_cast<uint8_t>(OpCode::OP_SET_GLOBAL):
                {
                    // Get the index of the variable and its name.
                    uint8_t index = curr_chunk_.data[ip_++];
                    std::string var_name = std::get<std::string>(curr_chunk_.constants.get_value(index));
                    
                    // The value we want to set should be on the stack.
                    Value value = stack_.front();
                    
                    bool exists = globals_.set_symbol(var_name, value);
                    if (!exists) {
                        globals_.remove_symbol(var_name);
                        
                        std::stringstream stream;
                        stream << "Undefined global variable: " << var_name;
                        throw RuntimeError(stream.str());
                    }
                }
                break;
            
            case static_cast<uint8_t>(OpCode::OP_EQUAL):
                {
                    Value a = stack_.front();
                    stack_.pop_front();
                    Value b = stack_.front();
                    stack_.pop_front();
                    stack_.push_front(values_equal_(a, b));
                }
                break;
                
            case static_cast<uint8_t>(OpCode::OP_GREATER):
                binary_op_(OpCode::OP_GREATER);
                break;
                
            case static_cast<uint8_t>(OpCode::OP_LESS):
                binary_op_(OpCode::OP_LESS);
                break;
                
            default:
                throw RuntimeError("Unknown opcode");
        }
    }
    
    return InterpretResult::INTERPRET_OK;
}

Value Vm::result() {
    if (stack_.empty()) {
        return {};
    }
    
    return stack_.front();
}

// Internal Helpers

void Vm::binary_op_(OpCode which_op) {
    auto b = stack_.front();
    stack_.pop_front();
    auto a = stack_.front();
    stack_.pop_front();
    
    switch (which_op) {
        case OpCode::OP_ADD:
            if (a.index() == 2 && b.index() == 2) {
                stack_.push_front(Value(std::get<double>(a) + std::get<double>(b)));
            } else if (a.index() == 3 && b.index() == 3) {
                stack_.push_front(Value(std::get<std::string>(a) + std::get<std::string>(b)));
            } else {
                throw RuntimeError("Can only add numbers and strings.");
            }
            break;
            
        case OpCode::OP_SUBTRACT:
            if (a.index() == 2 && b.index() == 2) {
                stack_.push_front(Value(std::get<double>(a) - std::get<double>(b)));
            } else {
                throw RuntimeError("Can only subtract numbers.");
            }
            break;
            
        case OpCode::OP_MULTIPLY:
            if (a.index() == 2 && b.index() == 2) {
                stack_.push_front(Value(std::get<double>(a) * std::get<double>(b)));
            } else {
                throw RuntimeError("Can only multiply numbers.");
            }
            break;
            
        case OpCode::OP_DIVIDE:
            if (a.index() == 2 && b.index() == 2) {
                stack_.push_front(Value(std::get<double>(a) / std::get<double>(b)));
            } else {
                throw RuntimeError("Can only divide numbers.");
            }
            break;
            
        case OpCode::OP_GREATER:
            {
                bool result = false;
                if (a > b) {
                    result = true;
                }
                
                stack_.push_front(result);
            }
            break;
            
        case OpCode::OP_LESS:
            {
                bool result = false;
                if (a < b) {
                    result = true;
                }
                
                stack_.push_front(result);
            }
            break;
            
        default:
            throw RuntimeError("Unkown operation");
            break;
    }
}

bool Vm::is_falsey_(const Value& value) {
    auto value_type = value.index();
    
    // Empty is false.
    if (value_type == 0) {
        return true;
    }
    
    // boolean, which can obviously be false.
    if (value_type == 1) {
        if (std::get<bool>(value) == false) {
            return true;
        }
    }
    
    // This is not false, it is "some" value.
    return false;
}

bool Vm::values_equal_(const Value& value1, const Value& value2) {
    if (value1.index() != value2.index()) {
        return false;
    }
    
    switch(value1.index()) {
        case 0:
            // Both are empty;
            return true;
            
        case 1:
            if (std::get<bool>(value1) == std::get<bool>(value2)) {
                return true;
            }
            break;
        
        case 2:
            if (std::get<double>(value1) == std::get<double>(value2)) {
                return true;
            }
            break;
            
        case 3:
            if (std::get<std::string>(value1) == std::get<std::string>(value2)) {
                return true;
            }
            break;
            
        default:
            return false;
    }
    
    return false;
}

} // namespace cpplox2
