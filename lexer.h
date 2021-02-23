// no multiple definitions of same file
#ifndef LEXER_H
#define LEXER_H

#include<iostream>
#include<vector>
#include<map>

/* ENUM FOR THE DIFFERENT TYPES OF TOKENS*/
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

/* NEEDS TO BE 'extern' SO IT DOESN'T DUPLICATE SYMBOL*/
extern std::map<TokenType,std::string> TokenTypeMap;

/* TOKEN TYPE */
struct Token {
    TokenType type;
    float value;
    Token(TokenType Type);
    Token(TokenType Type, float Value);
};

/* OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Token const &t);

/* LEXER CLASS */
class Lexer {
    private:
        void advance(); // increase the iterator and check for errors
        Token generateNumber(); // generate numbers by advancing through the chars
        // std::string::iterator itCode; // code iterator
        int cursorPos;
        std::string code; // keep track of the code so we know when we hit the end
        char currentChar; // keep track of the current character the iterator is pointing at
    public:
        Lexer(std::string Code); // init func. takes a string of code as input
        std::vector<Token> generateTokens(); // returns an array of tokens each with type and value (if any: see token declarations)
};

#endif