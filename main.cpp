#include<iostream>
#include "file.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

bool debug = 1;
bool interpreter = 0;

int main(int argc, char **argv) {
    SymbolTable smbt = SymbolTable();
    /* SET GLOBAL VARIABLES */
    smbt.set("TRUE",1);
    smbt.set("FALSE",0);
    if (argc == 1) {
        std::string expression;
        int c=0;
        while (1) {
            std::cout << ">>> ";
            std::getline(std::cin, expression);
            Lexer lexer = Lexer(expression);
            std::vector<Token> tokens = lexer.generateTokens();
            if (tokens.size() == 0) continue;
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
            }
            Parser parser = Parser(tokens);
            Node tree = parser.parse();
            if (debug) {
                std::cout << "AST: " << tree << "\n";
            }
            Interpreter interpreter = Interpreter(smbt);
            Variable result = interpreter.visit(tree);
            std::cout<<result << "\n";
        }
    } else { // file
        std::string fname(argv[1]);
        std::string data = readFile(fname);
		std::string cwd = getWD(fname);
        if (data == "ERROR") {
            throw std::runtime_error("Unable to open file " + fname);
        }
        Lexer lexer = Lexer(data);
        std::vector<Token> tokens = lexer.generateTokens();
        if (tokens.size() == 0) return 0;
        /*std::cout << "[";
        for (int i = 0 ; i < tokens.size(); i++) {
            Token token = tokens[i];
            if (i == tokens.size()-1)
                std::cout << token;
            else
                std::cout << token << ", ";
        }
        std::cout << "]\n";*/
        Parser parser = Parser(tokens);
        Node tree = parser.parse();
        Interpreter interpreter = Interpreter(smbt);
		interpreter.cwd = &cwd;
        Variable result = interpreter.visit(tree);
    }
    return 0;
}