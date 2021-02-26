#include<iostream>
#include "lexer.h"
#include "parser.h"

int main() {
    Lexer lexer = Lexer("354 + 456 - 124 * 43 / 234");
    std::vector<Token> tokens = lexer.generateTokens();
    for (Token token : tokens)
        std::cout << token << std::endl;
}