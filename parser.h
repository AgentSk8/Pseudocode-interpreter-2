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
    n_VAR_ASSIGN = 9,
    n_VAR_ACCESS = 10,
    n_AND = 11,
    n_OR = 12,
    n_NOT = 13,
    n_GT = 14,
    n_LT = 15,
    n_GTE = 16,
    n_LTE = 17,
    n_EE = 18,
    n_NE = 19,
    n_IF = 20,
    n_IF_ELSE = 21,
    n_FOR = 22,
    n_WHILE = 23,
    n_PRINT = 24,
    n_READ = 25,
    n_INPUT = 26,
    n_STRING = 27,
    n_LIST = 28,
    n_BLOCK = 29, // block of code - contains list of separate nodes
    n_DEF = 30,
    n_ARGS = 31,
    n_ARG = 32
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
    Node(NodeType Type, std::string Name); // for var access
    Node(NodeType Type); // null
    Node(); // for empty declaration in interpreter
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
        Node expr(); // calls comp_expr on each node and returns identifier / logic node
        Node comp_expr(); // calls arith_expr on each node and returns not / boolop node
        Node arith_expr(); // calls term on each node and returns + / - node
        Node term(); // calls factor on each node and returns ÷ / * node
        Node factor(); // +- factor
        Node power(); // atom ^ factor (right hand is less important)
        Node atom(); // returns either a number or the expr inside parens
        Node list_expr(); // [<expr> (COMMA)*]
        Node if_expr(); // IF <condition> THEN <block_expr> (ELSE <block_expr>) ENDIF
        Node for_expr(); // FOR <expr:assignment> TO <expr:end> <block_expr> NEXT <identifier>
        Node while_expr(); // WHILE <condition> DO <block_expr> ENDWHILE
        Node def_expr(); // DEF identifier LPAREN (identifier (COMMA))* DO <block_expr> RETURN <expr>
        Node builtin_expr(); // COMMAND (<expression> (COMMA))*
        Node block_expr(std::vector<std::string> terminators); // (expr(<sep_expr>*))* until one of terminator KEYWORDS reached. e.g. ENDIF/ELSE for IF
        int sep_expr(); // goes past any useless separators, returns the number of separators.
        void raiseError(); // in case of a syntax error (at the end of factor or after parse) throw runtime error
        void raiseError(std::string msg); // syntax error-specific message
        void advance(); // advance the cursor and check for end
        void devance(); // undo an advance
};
#endif