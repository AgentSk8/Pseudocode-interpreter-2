#include "lexer.h"

/* STRING CONSTANTS */
std::string DIGITS = "0123456789";
std::string WHITESPACE = " \n\t";

/* MAP USED FOR TOKENN "<<" OPERATOR OVERLOAD*/
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

/* TOKEN DECLARATION (DEPENDS ON TYPE) */
Token::Token(TokenType Type) {
    type = Type;
    value = 0;
}

Token::Token(TokenType Type, float Value) {
    type = Type;
    value = Value;
}

/* OVERLOAD "<<" OPERATOR FOR TOKEN */
std::ostream &operator<<(std::ostream &os, Token const &t) { 
    std::string type = TokenTypeMap[t.type];
    if (type == "NUMBER")
        return os << type << ": " << t.value;
    else
        return os << type;
}

Lexer::Lexer(std::string Code) {
    Code = ' ' + Code; // safety space as placeholder for advance (needs to init vars)
    cursorPos = 0;
    code = Code;
    Lexer::advance();
}

void Lexer::advance() {
    /* ADVANCE CURSOR POSITION */
    cursorPos++;
    currentChar = code[cursorPos];
    
    /* CHECK IF WE HAVE REACHED THE END AND ASSIGN A NULL CHAR*/
    if (cursorPos == code.size() - 1)
        currentChar = '\0';
}

Token Lexer::generateNumber() {
    /* ITERATE WHILE IS NUMBER OR DECIMAL */
    std::string numberStr = "";
    numberStr += currentChar;
    short dp_count = 0;

    advance();

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

std::vector<Token> Lexer::generateTokens() {
    std::vector<Token> tokens = {}; // array to later return

    while (currentChar != '\0') // while char is not null (see advance)
    {
        /* ADVANCE IF WHITESPACE */
        if (WHITESPACE.find(currentChar) != std::string::npos)
            advance();
        /* CHECK FOR NUMBERS */
        else if (currentChar == '.' or DIGITS.find(currentChar) != std::string::npos)
            tokens.push_back(generateNumber());
        /* CHECK FOR OPERATORS */
        else {
            char oldChar = currentChar;
            Lexer::advance();
            Token operatorToken = Token(TokenType::NONE);
            switch (oldChar) {
                case '+':
                    operatorToken = Token(TokenType::PLUS);
                    break;
                case '-':
                    operatorToken = Token(TokenType::MINUS);
                    break;
                case '*':
                    operatorToken = Token(TokenType::MULTIPLY);
                    break;
                case '/':
                    operatorToken = Token(TokenType::DIVIDE);
                    break;
                case '(':
                    operatorToken = Token(TokenType::LPAREN);
                    break;
                case ')':
                    operatorToken = Token(TokenType::RPAREN);
                    break;
                default: // unknown character
                    std::string msg = "Illegal character: ";
                    msg += oldChar;
                    throw std::runtime_error(msg);
            }
            tokens.push_back(operatorToken);
        }
    }
    return tokens;
}