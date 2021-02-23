#include "lexer.h"

std::string DIGITS = "0123456789";
std::string WHITESPACE = " \n\t";

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

std::ostream &operator<<(std::ostream &os, Token const &t) { 
    std::string type = TokenTypeMap[t.type];
    return os << type << ": " << t.value;
}

Lexer::Lexer(std::string Code) {
    Code = ' ' + Code; // safety space as placeholder for advance (needs to init vars)
    itCode = Code.begin();
    code = Code;
    Lexer::advance();
}

void Lexer::advance() {
    itCode++;
    currentChar = *itCode;

    if (itCode == code.end())
        currentChar = '\0';
}

Token Lexer::generateNumber() {
    /* ITERATE WHILE IS NUMBER OR DECIMAL */
    std::string numberStr = "";
    numberStr += currentChar;
    short dp_count = 0;

    Lexer::advance();

    while (currentChar != '\0' and (currentChar == '.' or DIGITS.find(currentChar) != std::string::npos))
    {
        if (currentChar == '.') {
            dp_count++;
            if (dp_count > 1) break;
        }

        numberStr += currentChar;
        advance();
    }

    /* SAFETY ZEROES */
    if (numberStr[0] == '.') numberStr = '0' + numberStr;
    if (numberStr[-1] == '.') numberStr += '0';

    /* CREATE AND RETURN A NUMBER TOKEN WITH FLOAT VALUE NUMBERSTR */
    return Token(TokenType::NUMBER, std::stof(numberStr));

}