#ifndef LEXER_H
#define LEXER_H

#include<iostream>
#include<vector>
#include<map>

enum TokenType{
    NUMBER = 0,
    PLUS = 1,
    MINUS = 2,
    MULTIPLY = 3,
    DIVIDE = 4,
    LPAREN = 5,
    RPAREN = 6,
    NONE = 7
};

extern std::map<TokenType,std::string> TokenTypeMap;

struct Token {
    TokenType type;
    float value;
    Token(TokenType Type);
    Token(TokenType Type, float Value);
};

std::ostream &operator<<(std::ostream &os, Token const &t);

class Lexer {
    public:
        void advance();
        Token generateNumber();
        std::string::iterator itCode; // code iterator
        std::string code;
        char currentChar;
    public:
        Lexer(std::string Code);
        std::vector<Token> generateTokens();
};

#endif