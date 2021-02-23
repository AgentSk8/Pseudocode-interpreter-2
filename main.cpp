#include<iostream>
#include "lexer.h"
int main() {
    Lexer lexer = Lexer("354");
    std::cout << lexer.generateNumber() << std::endl;
}