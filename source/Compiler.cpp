// Copyright 2026 Yasser Zabuair.  See LICENSE for details.
#include "Compiler.h"

#include "ParseError.h"

namespace cpplox2 {

Compiler::Compiler(const std::string& script,
                   const std::string& file_name):
    scanner_{script},
    file_name_{file_name} {
    
}

Chunk Compiler::compile() {
    curr_chunk_.clear();
    
    advance_();
    expression_();
    consume_(TokenType::SEMICOLON, "Expected semi-colon");
    
    return curr_chunk_;
}

// Internal Helpers

void Compiler::expression_() {
    parse_precendence_(Precedence::PREC_ASSIGNMENT);
}

void Compiler::number_() {
    emit_constant_(std::get<double>(prev_.literal));
}

void Compiler::unary_() {
    TokenType opType = prev_.type;
    
    parse_precendence_(Precedence::PREC_UNARY);
    
    switch (opType) {
        case TokenType::BANG:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_NOT));
            break;
        case TokenType::MINUS:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_SUBTRACT));
            break;
        default:
            return;
    }
}

void Compiler::parse_precendence_(Precedence precendence) {
    advance_();
    
    const auto& rule = get_rule_(prev_.type);
    if (!rule.prefix) {
        throw ParseError("Expected a prefix rule.",
                         file_name_,
                         scanner_.line());
    }
    
    rule.prefix();
    
    while(precendence <= get_rule_(curr_.type).precedence) {
        advance_();
        const auto& rule2 = get_rule_(prev_.type);
        if (!rule2.infix) {
            throw ParseError("Expected an infix rule.",
                             file_name_,
                             scanner_.line());
        }
        rule2.infix();
    }
}

void Compiler::grouping_() {
    expression_();
    consume_(TokenType::RIGHT_PAREN, "Expected ) after expression");
}

void Compiler::binary_() {
    TokenType operator_type = prev_.type;
    
    const ParseRule& rule = get_rule_(operator_type);
    parse_precendence_(static_cast<Precedence>(rule.precedence + 1));  // right associative.
    
    switch(operator_type) {
        case TokenType::BANG_EQUAL:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_EQUAL));
            emit_byte_(static_cast<uint8_t>(OpCode::OP_NOT));
            break;
            
        case TokenType::EQUAL_EQUAL:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_EQUAL));
            break
            ;
        case TokenType::GREATER:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_GREATER));
            break
            ;
        case TokenType::GREATER_EQUAL:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_LESS));
            emit_byte_(static_cast<uint8_t>(OpCode::OP_NOT));
            break;
            
        case TokenType::LESS:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_LESS));
            break;
            
        case TokenType::LESS_EQUAL:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_GREATER));
            emit_byte_(static_cast<uint8_t>(OpCode::OP_NOT));
            break;
            
        case TokenType::PLUS:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_ADD));
            break;
            
        case TokenType::MINUS:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_SUBTRACT));
            break;
            
        case TokenType::STAR:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_MULTIPLY));
            break;
            
        case TokenType::SLASH:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_DIVIDE));
            break;
            
        default:
            throw ParseError("Unknown token",
                             file_name_,
                             scanner_.line());
        return; // Unreachable.
    }
}

void Compiler::literal_() {
    switch (prev_.type) {
        case TokenType::FALSE:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_FALSE));
            break;
        case TokenType::TRUE:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_TRUE));
            break;
        case TokenType::NIL:
            emit_byte_(static_cast<uint8_t>(OpCode::OP_NIL));
            break;
        default:
            return;
    }
}

void Compiler::string_() {
    emit_constant_(std::get<std::string>(prev_.literal));
}

const Compiler::ParseRule& Compiler::get_rule_(TokenType token_type) {
    auto itr = rules_.find(token_type);
    if (itr == std::end(rules_)) {
        std::stringstream stream;
        stream << "Could not find rule for token: " << token_type;
        throw ParseError(stream.str(),
                         file_name_,
                         scanner_.line());
    }
    
    return itr->second;
}

void Compiler::emit_constant_(Value value) {
    int const_index = curr_chunk_.constants.add_value(value);
    if (const_index >= UINT8_MAX) {
        throw ParseError("Bro, you exceeded the number of constants allowed in a chunk.",
                         file_name_,
                         scanner_.line());
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
    
    throw ParseError(message,
                     file_name_,
                     scanner_.line());
}

void Compiler::emit_byte_(uint8_t byte) {
    curr_chunk_.write(byte, prev_.line);
}

} // namespace cpplox2
