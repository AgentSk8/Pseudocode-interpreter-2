#include "lexer.h"



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