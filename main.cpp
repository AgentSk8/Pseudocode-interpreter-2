#include<iostream>
#include "lexer.h"
int main() {
    Lexer lexer = Lexer();
    std::cout << "This works" << std::endl << lexer.a << std::endl;
}