#include<iostream>
#include "lexer.h"
int main() {
    Lexer lexer = Lexer("3 + 3");
    std::cout << "This works" << std::endl;
}