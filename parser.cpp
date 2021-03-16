#include "parser.h"
#include "lexer.h" // for Token

/* NODE DATACLASS */
/**
* 1. init node with child nodes
* 2. init value node
* 3. for nul type
*/
Node::Node(NodeType Type, std::vector<Node> Nodes){
    type = Type;
    nodes = Nodes;
}
Node::Node(NodeType Type, std::string Name,std::vector<Node> Nodes){
    type = Type;
    name = Name;
    nodes = Nodes;
}
Node::Node(NodeType Type, std::string Name){
    type = Type;
    name = Name;
}
Node::Node(NodeType Type, float Value){
    type = Type;
    value = Value;
}
Node::Node(NodeType Type){
    type = Type;
}

/* NODE OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Node const &n) {
    if (n.type == NodeType::n_NULL) return os << " "; // space if none
    else if (n.type == NodeType::n_NUMBER) return os << n.value; // value if number
    else {
        NodeType t = n.type;
        // NOTE: probably a better way to write these statements
        if (t == NodeType::n_ADD) return os << "(" << n.nodes[0] << "+" << n.nodes[1] << ")";
        else if (t == NodeType::n_SUBTRACT) return os << "(" << n.nodes[0] << "-" << n.nodes[1] << ")";
        else if (t == NodeType::n_MULTIPLY) return os << "(" << n.nodes[0] << "*" << n.nodes[1] << ")";
        else if (t == NodeType::n_DIVIDE) return os << "(" << n.nodes[0] << "/" << n.nodes[1] << ")";
        else if (t == NodeType::n_POWER) return os << "(" << n.nodes[0] << "^" << n.nodes[1] << ")";
        else if (t == NodeType::n_PLUS) return os << "(+" << n.nodes[0] << ")";
        else if (t == NodeType::n_MINUS) return os << "(-" << n.nodes[0] << ")";
        else if (t == NodeType::n_VAR_ASSIGN) return os <<"("<< n.name << "=" << n.nodes[0] << ")";
        else if (t == NodeType::n_VAR_ACCESS) return os << n.name;
        else return os << "?"; // otherwise return unknown
    }
};

/* PARSER CLASS */
// see grammar.txt to understand the rules. functions will be 
// added/updated in the future to account for different orders

Parser::Parser(std::vector<Token> t){
    tokens = t;
    cursorPos = 0;
    currentToken = tokens[0];
}
Node Parser::parse(){
    if (currentToken.type == TokenType::t_NONE) return Node(NodeType::n_NULL);

    Node result = expr(); // call least priority (expr +/-)

    // If token is left unparsed, raise syntax error
    if (currentToken.type != TokenType::t_NONE) raiseError(); 

    return result; // return parent / root node
}

Node Parser::expr() {
    // check for var declaration
    if (currentToken.type == TokenType::t_IDENTIFIER) {
        Token identifierToken = currentToken;
        advance();
        if (currentToken.type == TokenType::t_EQ) {
            advance();
            std::vector<Node> children = {expr()};
            return Node(NodeType::n_VAR_ASSIGN, identifierToken.name, children);
        } else {
            devance();
        }
    }

    Node result = term(); // next top priority (term */÷)

    while (currentToken.type == TokenType::t_MINUS or currentToken.type == TokenType::t_PLUS) { // so as long as we are in a sum / subtraction
        if (currentToken.type == TokenType::t_PLUS) {
            advance();

            std::vector<Node> children = {result, term()}; // child nodes of add node
            result = Node(NodeType::n_ADD, children);
        } else if (currentToken.type == TokenType::t_MINUS) {
            advance();

            std::vector<Node> children = {result, term()}; // child nodes of subtract node
            result = Node(NodeType::n_SUBTRACT, children);
        }
    }
    return result;
}
Node Parser::term() {
    Node result = factor(); // next (and final) top priority (factor number/parens)
    while (currentToken.type == TokenType::t_DIVIDE or currentToken.type == TokenType::t_MULTIPLY) { // while is division or multiplication
        if (currentToken.type == TokenType::t_MULTIPLY) {
            advance();
            std::vector<Node> children = {result, factor()}; // child nodes of multiply node
            result = Node(NodeType::n_MULTIPLY, children);
        } else if (currentToken.type == TokenType::t_DIVIDE) {
            advance();
            std::vector<Node> children = {result, factor()}; // child nodes of divide node
            result = Node(NodeType::n_DIVIDE, children);
        }
    }
    return result;
}
Node Parser::factor() {
    Token oldToken = currentToken; // need to create a copy because we advance later

    if (oldToken.type == TokenType::t_PLUS) {
        advance();
        std::vector<Node> plusValue = {factor()}; // call factor as it will either be +number or +() parentheses
        return Node(NodeType::n_PLUS, plusValue);
    } else if (oldToken.type == TokenType::t_MINUS) {
        advance();
        std::vector<Node> minusValue = {factor()}; // call factor as it will either be -number or -() parentheses
        return Node(NodeType::n_MINUS, minusValue);
    }
    return power();
}
Node Parser::power() {
    Node result = atom();
    if (currentToken.type == TokenType::t_POW) {
        advance();
        return Node(NodeType::n_POWER, std::vector<Node> {result, factor()});
    }
    return result;
}
Node Parser::atom() {
    Token oldToken = currentToken;

    if (oldToken.type == TokenType::t_LPAREN) {
        advance(); // advance into expression inside paren
        Node result = expr(); // evaluate this as a new expression
        if (currentToken.type != TokenType::t_RPAREN) {
            raiseError(); // raise syntax error because parentheses haven't been closed
        }
        advance();
        return result;
    } else if (oldToken.type == TokenType::t_NUMBER) {
        advance();
        return Node(NodeType::n_NUMBER, oldToken.value); // return node of type number with the token's value
    } else if (oldToken.type == TokenType::t_IDENTIFIER) { // access the identifier
        advance();
        return Node(NodeType::n_VAR_ACCESS, oldToken.name);
    }
    raiseError(); // not any of the token types or invalid grammar

}
void Parser::raiseError() {
    throw std::runtime_error("Invalid Syntax.");
}
void Parser::advance() {
    // NOTE: this would also have to be updated to use iterators
    cursorPos++;
    if (cursorPos < tokens.size()) currentToken = tokens[cursorPos];
    else currentToken = Token(TokenType::t_NONE);
}
void Parser::devance() {
    // NOTE: this would also have to be updated to use iterators
    cursorPos--;
    if (cursorPos >= 0) currentToken = tokens[cursorPos];
    else throw std::runtime_error("Invalid devance.");
}