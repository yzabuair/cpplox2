// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once
#include "Chunk.h"
#include "Scanner.h"


#include <exception>
#include <functional>
#include <map>
#include <source_location>
#include <sstream>
#include <string>

namespace cpplox2 {

/// Compiles the code into our virtual machine format.
class Compiler {
    Scanner scanner_;
    std::string file_name_;
    Token prev_;
    Token curr_;
    Chunk curr_chunk_;
    
    enum Precedence {
        PREC_NONE,
        PREC_ASSIGNMENT,    // =
        PREC_OR,            // or
        PREC_AND,           // and
        PREC_EQUALITY,      // == !=
        PREC_COMPARISON,    // < > <= >=
        PREC_TERM,          // + -
        PREC_FACTOR,        // * /
        PREC_UNARY,         // ! -
        PREC_CALL,          // . ()
        PREC_PRIMARY
    };
    
    struct ParseRule {
        std::function<void ()>  prefix;
        std::function<void ()>  infix;
        Precedence precedence;
    };
    
    const std::map<TokenType, ParseRule> rules_ = {
        {TokenType::LEFT_PAREN,     {[this](){ this->grouping_(); }, nullptr, Precedence::PREC_NONE}},
        {TokenType::RIGHT_PAREN,    {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::LEFT_BRACE,     {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::RIGHT_BRACE,    {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::COMMA,          {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::DOT,            {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::MINUS,          {[this](){ this->unary_(); }, [this](){ this->binary_(); }, Precedence::PREC_TERM}},
        {TokenType::PLUS,           {nullptr, [this](){ binary_(); },  Precedence::PREC_TERM}},
        {TokenType::SEMICOLON,      {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::SLASH,          {nullptr, [this](){ binary_(); }, Precedence::PREC_FACTOR}},
        {TokenType::STAR,           {nullptr, [this](){ binary_(); }, Precedence::PREC_FACTOR}},
        {TokenType::BANG,           {[this]() { this->unary_(); }, nullptr, Precedence::PREC_NONE}},
        {TokenType::BANG_EQUAL,     {nullptr, [this](){ this->binary_(); }, Precedence::PREC_EQUALITY}},
        {TokenType::EQUAL,          {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::EQUAL_EQUAL,    {nullptr, [this](){ this->binary_(); }, Precedence::PREC_EQUALITY}},
        {TokenType::GREATER,        {nullptr, [this](){ this->binary_(); }, Precedence::PREC_EQUALITY}},
        {TokenType::GREATER_EQUAL,  {nullptr, [this](){ this->binary_(); }, Precedence::PREC_EQUALITY}},
        {TokenType::LESS,           {nullptr, [this](){ this->binary_(); }, Precedence::PREC_EQUALITY}},
        {TokenType::LESS_EQUAL,     {nullptr, [this](){ this->binary_(); }, Precedence::PREC_EQUALITY}},
        {TokenType::IDENTIFIER,     {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::STRING,         {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::NUMBER,         {[this](){ number_(); }, nullptr, Precedence::PREC_NONE}},
        {TokenType::AND,            {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::CLASS,          {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::ELSE,           {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::FALSE,          {[this](){ this->literal_(); }, nullptr, Precedence::PREC_NONE}},
        {TokenType::FOR,            {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::FUN,            {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::IF,             {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::NIL,            {[this](){ this->literal_(); }, nullptr, Precedence::PREC_NONE}},
        {TokenType::OR,             {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::PRINT,          {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::RETURN,         {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::SUPER,          {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::THIS,           {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::TRUE,           {[this](){ this->literal_(); }, nullptr, Precedence::PREC_NONE}},
        {TokenType::VAR,            {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::WHILE,          {nullptr, nullptr, Precedence::PREC_NONE}},
        {TokenType::ENDOFFILE,      {nullptr, nullptr, Precedence::PREC_NONE}},
    };
    
public:
    Compiler(const std::string& script,
             const std::string& file_name = "<stdin>");
    ~Compiler() = default;
    
    Chunk compile();
    
// Internal Helpers
private:
    void expression_();
    void number_();
    void unary_();
    void parse_precendence_(Precedence precendence);
    void grouping_();
    void binary_();
    void literal_();
    const ParseRule& get_rule_(TokenType token_type);
    void emit_constant_(Value value);
    void advance_();
    void consume_(TokenType type, const std::string& message);
    void emit_byte_(uint8_t byte);
    
    
};
} // namespace cpplox2
