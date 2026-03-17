// Copyright 2026 Yasser Zabuair.  See LICENSE for details.
#include "Compiler.h"

namespace cpplox2 {

Compiler::Compiler(const std::string& script):
    scanner_{script} {
    
}

Chunk Compiler::compile() {
    
}

// Internal Helpers

void Compiler::expression_() {
    parse_precendence_(Precedence::PREC_ASSIGNMENT);
}

void Compiler::number_() {
    emit_constant_(std::get<2>(prev_.literal));
}

void Compiler::unary_() {
    TokenType opType = prev_.type;
    
    parse_precendence_(Precedence::PREC_UNARY);
    
    switch (opType) {
        case TokenType::MINUS:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_SUBTRACT));
            break;
        default:
            return;
    }
}

void Compiler::parse_precendence_(Precedence precendence) {
    
}

void Compiler::grouping_() {
    expression_();
    consume_(TokenType::RIGHT_PAREN, "Expected ) after expression");
}

void Compiler::binary_() {
    TokenType operator_type = prev_.type;
    
    // ParseRule& rule = get_rule_(operator_type);
    
}

const Compiler::ParseRule& Compiler::get_rule_(TokenType token_type) {
    auto itr = rules_.find(token_type);
    if (itr == std::end(rules_)) {
        // TODO: Error..
    }
    
    return itr->second;
}

void Compiler::emit_constant_(Value value) {
    int const_index = curr_chunk_.constants.add_value(value);
    if (const_index >= UINT8_MAX) {
        // TODO Error..
    }
    emit_byte_(static_cast<uint8_t>(OpCode::OP_CONSTANT));
    emit_byte_(const_index);
}

void Compiler::advance_() {
    prev_ = curr_;
    curr_ = scanner_.scan_token();
}

void Compiler::consume_(TokenType type, const std::string& message) {
    if (curr_.type == type) {
        advance_();
        return;
    }
    
    // TODO: Throw an error of some sort.
}

void Compiler::emit_byte_(uint8_t byte) {
    curr_chunk_.write(byte, prev_.line);
}

} // namespace cpplox2
