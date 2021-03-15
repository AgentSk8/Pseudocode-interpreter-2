#ifndef PARSER_H
#define PARSER_H

#include<iostream>
#include<vector>
#include "lexer.h"

/* NODETYPE ENUM */
enum NodeType {
    n_NULL = 0,
    n_NUMBER = 1,
    n_ADD = 2,
    n_SUBTRACT = 3,
    n_DIVIDE = 4,
    n_MULTIPLY = 5,
    n_POWER = 6,
    n_PLUS = 7,
    n_MINUS = 8,
    n_VAR_ASSIGN
};

/* NODE STRUCT */
struct Node {
    NodeType type;
    float value;
    std::vector<Node> nodes;
    std::string name;
    Node(NodeType Type, std::vector<Node> Nodes); // for all other nodes
    Node(NodeType Type, float Value); // for factor = number
    Node(NodeType Type, std::string Name,std::vector<Node> Nodes); // for var assign
    Node(NodeType Type); // null
};

/* OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Node const &n);


/* PARSER CLASS */
class Parser {
    public:
        Parser(std::vector<Token> Tokens); // init
        Node parse(); // parse calls expr
    private:
        std::vector<Token> tokens; // token array returned from lexer
        Token currentToken = Token(TokenType::t_NONE); // current token
        int cursorPos; // current token index
        Node expr(); // calls term on each node and returns + / - node
        Node term(); // calls factor on each node and returns ÷ / * node
        Node factor(); // +- factor
        Node power(); // atom ^ factor (right hand is less important)
        Node atom(); // returns either a number or the expr inside parens
        void raiseError(); // in case of a syntax error (at the end of factor or after parse) throw runtime error
        void advance(); // advance the cursor and check for end
};
#endif