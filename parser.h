#ifndef PARSER_H
#define PARSER_H

#include<iostream>
#include<vector>
#include "lexer.h"

/* NODETYPE ENUM */
enum NodeType {
    NULL = 0,
    NUMBER = 1,
    ADD = 2,
    SUBTRACT = 3,
    DIVIDE = 4,
    MULTIPLY = 5,
    PLUS = 6,
    MINUS = 7
};

/* NODE STRUCT */
struct Node {
    NodeType type;
    float value;
    std::vector<Node> nodes;

    Node(NodeType Type, std::vector<Node> Nodes); // for all other nodes
    Node(NodeType Type, float value); // for factor = number
};

/* OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Node const &n);

/* PARSER CLASS */
class Parser {
    public:
        std::vector<Token> tokens; // token array returned from lexer
        Token currentToken; // current token
        int cursorPos; // current token index

        Parser(std::vector<Token> Tokens); // init
        Node parse(); // parse calls expr
    private:
        Node expr(); // calls term on each node and returns + / - node
        Node term(); // calls factor on each node and returns ÷ / * node
        Node factor(); // either returns a number, or whatever expr returns for inside parens
        void raiseError(); // in case of a syntax error (at the end of factor or after parse) throw runtime error
        void advance(); // advance the cursor and check for end
};
#endif