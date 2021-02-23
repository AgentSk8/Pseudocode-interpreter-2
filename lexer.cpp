#include "lexer.h"

std::map<TokenType,std::string> TokenTypeMap = {
    {NUMBER,"NUMBER"},
    {PLUS,"PLUS"},
    {MINUS,"MINUS"},
    {MULTIPLY,"MULTIPLY"},
    {DIVIDE,"DIVIDE"},
    {LPAREN,"LPAREN"},
    {RPAREN,"RPAREN"},
    {NONE,"NONE"}
};

Token::Token(TokenType Type) {
    type = Type;
    value = 0;
}

Token::Token(TokenType Type, float Value) {
    type = Type;
    value = Value;
}

Lexer::Lexer(std::string Code) {
    itCode = Code.begin();
    code = Code;
    Lexer::advance();
}

void Lexer::advance() {
    return;
}