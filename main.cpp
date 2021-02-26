#include<iostream>
#include "lexer.h"
#include "parser.h"

int main() {
    std::string expression;
    std::cout << ">>> ";
    std::getline(std::cin, expression);
    Lexer lexer = Lexer(expression);
    std::vector<Token> tokens = lexer.generateTokens();
    std::cout << "[";
    for (int i = 0 ; i < tokens.size(); i++) {
        Token token = tokens[i];
        if (i == tokens.size()-1)
            std::cout << token;
        else
            std::cout << token << ", ";
    }
    std::cout << "]\n";
    Parser parser = Parser(tokens);
    Node tree = parser.parse();
    std::cout << "AST: " << tree << "\n";
}