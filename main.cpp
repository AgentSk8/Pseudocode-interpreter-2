#include<iostream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

bool debug = false;

std::string expression;
int main() {
    SymbolTable smbt = SymbolTable();
    /* SET GLOBAL VARIABLES */
    smbt.set("TRUE",1);
    smbt.set("FALSE",0);

    int c=0;
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
        Interpreter interpreter = Interpreter(smbt);
        Variable result = interpreter.visit(tree);
        std::cout << result.number << "\n";
    }
}