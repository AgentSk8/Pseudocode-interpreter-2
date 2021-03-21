#include "lexer.h"
#include <vector>

/* STRING CONSTANTS */
std::string DIGITS = "0123456789";
std::string WHITESPACE = " \n\t";
std::string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* MAP USED FOR TOKEN "<<" OPERATOR OVERLOAD*/
std::map<TokenType,std::string> TokenTypeMap = {
    {t_NUMBER,"NUMBER"},
    {t_PLUS,"PLUS"},
    {t_MINUS,"MINUS"},
    {t_MULTIPLY,"MULTIPLY"},
    {t_DIVIDE,"DIVIDE"},
    {t_POW,"POW"},
    {t_LPAREN,"LPAREN"},
    {t_RPAREN,"RPAREN"},
    {t_IDENTIFIER, "IDENTIFIER"},
    {t_EQ, "EQ"},
    {t_NONE,"NONE"},
    {t_EE,"BOOL_EQ"},
    {t_NE,"NOT_EQ"},
    {t_LT,"LT"},
    {t_GT,"GT"},
    {t_LTE,"LTE"},
    {t_GTE,"GTE"},
    {t_KEYWORD,"KEYWORD"}
};

/* VECTOR TO STORE KEYWORDS */
std::vector<std::string> keywords = {
    "AND",
    "OR",
    "NOT",
    "IF",
    "THEN",
    "ELSE",
    "ENDIF",
    "FOR",
    "TO",
    "NEXT",
    "WHILE",
    "DO",
    "ENDWHILE",
    "PRINT",
    "READ"
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

Token::Token(TokenType Type, std::string Name) {
    type = Type;
    name = Name;
}

/* OVERLOAD "<<" OPERATOR FOR TOKEN */
std::ostream &operator<<(std::ostream &os, Token const &t) { 
    std::string type = TokenTypeMap[t.type];
    if (type == "NUMBER")
        return os << type << ": " << t.value;
    else if (type == "IDENTIFIER" or type == "KEYWORD")
        return os << type << ": " << t.name;
    else
        return os << type;
}

Lexer::Lexer(std::string Code) {
    // NOTE: need to fix so no need for 'safety space'
    Code = ' ' + Code + ' '; // safety space as placeholder for advance (needs to init vars) and at end
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
    return Token(TokenType::t_NUMBER, std::stof(numberStr));

}

Token Lexer::generateWord() {
    std::string wordStr = "";
    while (ALPHABET.find(currentChar) != std::string::npos) {
        wordStr += currentChar;
        advance();
    }
    // give keywords priority
    if (std::find(keywords.begin(), keywords.end(), wordStr) != keywords.end())
        return Token(TokenType::t_KEYWORD, wordStr);
    else
        return Token(TokenType::t_IDENTIFIER, wordStr);
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
        /* IF LETTERS, GENERATE IDENTIFIER OR KEYWORD*/
        else if (ALPHABET.find(currentChar) != std::string::npos)
            tokens.push_back(generateWord());
        /* CHECK FOR OPERATORS */
        else {
            char oldChar = currentChar;
            Lexer::advance();
            Token operatorToken = Token(TokenType::t_NONE);
            switch (oldChar) {
                case '+':
                    operatorToken = Token(TokenType::t_PLUS);
                    break;
                case '-':
                    operatorToken = Token(TokenType::t_MINUS);
                    break;
                case '*':
                    operatorToken = Token(TokenType::t_MULTIPLY);
                    break;
                case '/':
                    operatorToken = Token(TokenType::t_DIVIDE);
                    break;
                case '^':
                    operatorToken = Token(TokenType::t_POW);
                    break;
                case '(':
                    operatorToken = Token(TokenType::t_LPAREN);
                    break;
                case ')':
                    operatorToken = Token(TokenType::t_RPAREN);
                    break;
                case '<':
                    if (currentChar == '-') {
                        advance();
                        operatorToken = Token(TokenType::t_EQ);
                    } else if (currentChar == '>') {
                        advance();
                        operatorToken = Token(TokenType::t_NE);
                    } else if (currentChar == '=') {
                        advance();
                        operatorToken = Token(TokenType::t_LTE);
                    } else {
                        operatorToken = Token(TokenType::t_LT);
                    }
                    break;
                case '>':
                    if (currentChar == '=') {
                        advance();
                        operatorToken = Token(TokenType::t_GTE);
                    } else {
                        operatorToken = Token(TokenType::t_GT);
                    }
                    break;
                case '=':
                    operatorToken = Token(TokenType::t_EE);
                    break;
                case ',':
                    operatorToken = Token(TokenType::t_COMMA);
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