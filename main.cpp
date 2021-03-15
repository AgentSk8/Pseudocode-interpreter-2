#include<iostream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

bool debug = true;
std::string expression;
int main() {
    while (1) {
        std::cout << ">>> ";
        std::getline(std::cin, expression);

        Lexer lexer = Lexer(expression);
        std::vector<Token> tokens = lexer.generateTokens();

        Parser parser = Parser(tokens);
        Node tree = parser.parse();

        /* DEBUGGING LEXER AND PARSER */
        if (debug) {
            std::cout << "[";
            for (int i = 0 ; i < tokens.size(); i++) {
                Token token = tokens[i];
                if (i == tokens.size()-1)
                    std::cout << token;
                else
                    std::cout << token << ", ";
            }
            std::cout << "]\n";
            std::cout << "AST: " << tree << "\n";
        }
        
        // Interpreter interpreter = // Interpreter();
        // Number result = // interpreter.visit(tree);

        // std::cout << result << "\n";
    }
}