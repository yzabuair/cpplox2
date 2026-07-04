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
    while(!match_(TokenType::ENDOFFILE)) {
        declaration_();
    }
    
    return curr_chunk_;
}

// Internal Helpers

void Compiler::expression_() {
    parse_precendence_(Precedence::PREC_ASSIGNMENT);
}

void Compiler::expression_statement_() {
    expression_();
    consume_(TokenType::SEMICOLON, "Expected semi-colon after expression statement.");
    emit_byte_(OpCode::OP_POP);
}

void Compiler::print_statement_() {
    expression_();
    consume_(TokenType::SEMICOLON, "Expect ';' after value.");
    emit_byte_(OpCode::OP_PRINT);
}

void Compiler::var_declaration_() {
    uint8_t global_index = parse_variable_("Expected variable name");
    
    if (match_(TokenType::EQUAL)) {
        expression_();
    } else {
        emit_byte_(OpCode::OP_NIL);
    }
    
    consume_(TokenType::SEMICOLON, "Expected semi-colon at end of var declaration.");
    
    define_variable_(global_index);
}

uint8_t Compiler::parse_variable_(const std::string& err_msg) {
    consume_(TokenType::IDENTIFIER, err_msg);
    
    declare_variable_();
    if (scope_depth_ > 0) {
        return 0;
    }
    
    return identifier_constant_(prev_);
}

void Compiler::define_variable_(uint8_t index) {
    if (scope_depth_ > 0) {
        return;
    }
    
    emit_byte_(OpCode::OP_DEFINE_GLOBAL);
    emit_byte_(index);
}

uint8_t Compiler::identifier_constant_(const Token& name) {
    int const_index = curr_chunk_.constants.add_value(name.lexeme);
    if (const_index >= UINT8_MAX) {
        throw ParseError("Bro, you exceeded the number of constants allowed in a chunk.",
                         file_name_,
                         scanner_.line());
    }
    return const_index;
}

void Compiler::declare_variable_() {
    if (scope_depth_ == 0) {
        return;
    }
    
    for(const auto& curr_local: locals_) {
        if (curr_local.name.lexeme == prev_.lexeme && curr_local.depth == scope_depth_) {
            std::stringstream msg;
            msg << "Bitch, you already declared this variable: " << prev_.lexeme;
            throw ParseError(msg.str(),
                             file_name_,
                             scanner_.line());
        }
    }
    
    add_local_(prev_);
}

void Compiler::add_local_(const Token& token) {
    Local new_local {
        .name = token,
        .depth = scope_depth_
    };
    locals_.push_back(new_local);
}

void Compiler::declaration_() {
    if (match_(TokenType::VAR)) {
        var_declaration_();
    } else {
        statement_();
    }
}

void Compiler::statement_() {
    if (match_(TokenType::PRINT)) {
        print_statement_();
    } else if (match_(TokenType::LEFT_BRACE)) {
        begin_scope_();
        block_();
        end_scope_();
    } else {
        expression_statement_();
    }
}

void Compiler::block_() {
    while (!check_(TokenType::RIGHT_BRACE) && !check_(TokenType::ENDOFFILE)) {
        declaration_();
    }
    
    consume_(TokenType::RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::begin_scope_() {
    ++scope_depth_;
}

void Compiler::end_scope_() {
    --scope_depth_;
    
    // Kill all local variables that are not in scope.
    for(auto itr = std::begin(locals_); itr != std::end(locals_); ++itr) {
        if (itr->depth > scope_depth_) {
            itr = locals_.erase(itr);
            emit_byte_(OpCode::OP_POP);
        }
    }
}

void Compiler::number_() {
    emit_constant_(std::get<double>(prev_.literal));
}

void Compiler::unary_() {
    TokenType opType = prev_.type;
    
    parse_precendence_(Precedence::PREC_UNARY);
    
    switch (opType) {
        case TokenType::BANG:
            emit_byte_(OpCode::OP_NOT);
            break;
        case TokenType::MINUS:
            emit_byte_(OpCode::OP_SUBTRACT);
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
    
    bool can_assign = precendence <= Precedence::PREC_ASSIGNMENT;
    rule.prefix(can_assign);
    
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
            emit_byte_(OpCode::OP_FALSE);
            break;
        case TokenType::TRUE:
            emit_byte_(OpCode::OP_TRUE);
            break;
        case TokenType::NIL:
            emit_byte_(OpCode::OP_NIL);
            break;
        default:
            return;
    }
}

void Compiler::string_() {
    emit_constant_(std::get<std::string>(prev_.literal));
}

void Compiler::variable_(bool can_assign) {
    named_variable_(prev_, can_assign);
}

void Compiler::named_variable_(const Token& name, bool can_assign) {
    uint8_t index = identifier_constant_(name);
    if (can_assign && match_(TokenType::EQUAL)) {
        expression_();
        emit_byte_(OpCode::OP_SET_GLOBAL);
        emit_byte_(index);
    } else {
        emit_byte_(OpCode::OP_GET_GLOBAL);
        emit_byte_(index);
    }
}

int Compiler::resolve_local_(const Token& name) {
    int i = 0;
    for(const auto& curr: locals_) {
        if (curr.depth == scope_depth_ && name.lexeme == curr.name.lexeme) {
            return i;
        }
        ++i;
    }
    
    return -1;
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

uint8_t Compiler::emit_constant_(Value value) {
    int const_index = curr_chunk_.constants.add_value(value);
    if (const_index >= UINT8_MAX) {
        throw ParseError("Bro, you exceeded the number of constants allowed in a chunk.",
                         file_name_,
                         scanner_.line());
    }
    emit_byte_(OpCode::OP_CONSTANT);
    emit_byte_(const_index);
    
    return const_index;
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

bool Compiler::check_(const TokenType& type) {
    return curr_.type == type;
}

bool Compiler::match_(const TokenType& type) {
    if (!check_(type)) {
        return false;
    }
    
    advance_();
    
    return true;
}

void Compiler::emit_byte_(OpCode code) {
    emit_byte_(static_cast<uint8_t>(code));
}

void Compiler::emit_byte_(uint8_t byte) {
    curr_chunk_.write(byte, prev_.line);
}

} // namespace cpplox2
