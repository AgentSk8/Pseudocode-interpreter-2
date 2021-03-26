// no multiple definitions of same file
#ifndef LEXER_H
#define LEXER_H

#include<iostream>
#include<vector>
#include<map>

/* ENUM FOR THE DIFFERENT TYPES OF TOKENS*/
enum TokenType{
    t_NUMBER = 0,
    t_PLUS = 1,
    t_MINUS = 2,
    t_MULTIPLY = 3,
    t_DIVIDE = 4,
    t_POW = 5,
    t_LPAREN = 6,
    t_RPAREN = 7,
    t_EQ = 8,
    t_IDENTIFIER = 9,
    t_NONE = 10,
    t_EE = 11, // boolean
    t_NE = 12,
    t_LT = 13,
    t_GT = 14,
    t_LTE = 15, // less than or equal to
    t_GTE = 16,
    t_KEYWORD = 17,
    t_COMMA = 18,
    t_STRING = 19
};

/* NEEDS TO BE 'extern' SO IT DOESN'T DUPLICATE SYMBOL*/
extern std::map<TokenType,std::string> TokenTypeMap;

/* TOKEN TYPE */
struct Token {
    TokenType type;
    float value;
    std::string name;
    Token(TokenType Type);
    Token(TokenType Type, float Value);
    Token(TokenType Type, std::string Name);
};

/* OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Token const &t);

/* LEXER CLASS */
class Lexer {
    private:
        void advance(); // increase the iterator and check for errors
        Token generateNumber(); // generate numbers by advancing through the chars
        Token generateWord(); // generate identifiers / keywords by advancing till not alphabet
        Token generateString(); // generat string by escaping characters and add all others
        int cursorPos;
        std::string code; // keep track of the code so we know when we hit the end
        char currentChar; // keep track of the current character the iterator is pointing at
    public:
        Lexer(std::string Code); // init func. takes a string of code as input
        std::vector<Token> generateTokens(); // returns an array of tokens each with type and value (if any: see token declarations)
};

#endif