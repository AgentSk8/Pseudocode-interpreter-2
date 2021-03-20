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
        else if (t == NodeType::n_AND) return os << "(" << n.nodes[0] << " && " << n.nodes[1] << ")";
        else if (t == NodeType::n_OR) return os << "(" << n.nodes[0] << " || " << n.nodes[1] << ")";
        else if (t == NodeType::n_NOT) return os << "!(" << n.nodes[0] << ")";
        else if (t == NodeType::n_GT) return os << "(" << n.nodes[0] << ">" << n.nodes[1] << ")";
        else if (t == NodeType::n_LT) return os << "(" << n.nodes[0] << "<" << n.nodes[1] << ")";
        else if (t == NodeType::n_GTE) return os << "(" << n.nodes[0] << ">=" << n.nodes[1] << ")";
        else if (t == NodeType::n_LTE) return os << "(" << n.nodes[0] << "<=" << n.nodes[1] << ")";
        else if (t == NodeType::n_EE) return os << "(" << n.nodes[0] << "==" << n.nodes[1] << ")";
        else if (t == NodeType::n_NE) return os << "(" << n.nodes[0] << "!=" << n.nodes[1] << ")";
        else if (t == NodeType::n_IF) return os << "(" << n.nodes[0] << "?" << n.nodes[1] << ")";
        else if (t == NodeType::n_IF_ELSE) return os << "(" << n.nodes[0] << "?" << n.nodes[1] << ":" << n.nodes[2] << ")"; 
        else if (t == NodeType::n_FOR) return os << "(for(" << n.nodes[0] << "->" << n.nodes[1] << ")::(" << n.nodes[2] << ")" << "next)";
        else if (t == NodeType::n_WHILE) return os << "(while(" << n.nodes[0] << ")::(" << n.nodes[1] << ")" << "next)";
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

    Node result = comp_expr(); // comparison expressions

    while (currentToken.type == TokenType::t_KEYWORD) {
        if (currentToken.name == "AND") {
            advance();

            std::vector<Node> children = {result, comp_expr()}; // remember rule is compexpr and/or compexpr
            result = Node(NodeType::n_AND, children);
        } else if (currentToken.name == "OR") {
            advance();

            std::vector<Node> children = {result, comp_expr()};
            result = Node(NodeType::n_OR, children);
        } else break;
    }

    return result;
    
}

Node Parser::comp_expr() {
    if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "NOT") {
        advance();
        std::vector<Node> children = { comp_expr() }; // NOT (comparison)
        return Node(NodeType::n_NOT, children);
    }
    Node result = arith_expr(); // arith expr (>|<|=|<>) arith expr
    std::vector<TokenType> boolOps = {TokenType::t_GT, TokenType::t_GTE, TokenType::t_LT, TokenType::t_LTE, TokenType::t_EE, TokenType::t_NE};
    while (std::find(boolOps.begin(),boolOps.end(),currentToken.type) != boolOps.end()) { // while boolop
        switch (currentToken.type) {
            case TokenType::t_GT: {
                advance();
                std::vector<Node> children = {result, arith_expr()};
                result = Node(NodeType::n_GT, children);
                break;
            }
            case TokenType::t_LT: {
                advance();
                std::vector<Node> children = {result, arith_expr()};
                result = Node(NodeType::n_LT, children);
                break;
            }
            case TokenType::t_GTE: {
                advance();
                std::vector<Node> children = {result, arith_expr()};
                result = Node(NodeType::n_GTE, children);
                break;
            }
            case TokenType::t_LTE: {
                advance();
                std::vector<Node> children = {result, arith_expr()};
                result = Node(NodeType::n_LTE, children);
                break;
            }
            case TokenType::t_EE: {
                advance();
                std::vector<Node> children = {result, arith_expr()};
                result = Node(NodeType::n_EE, children);
                break;
            }
            case TokenType::t_NE: {
                advance();
                std::vector<Node> children = {result, arith_expr()};
                result = Node(NodeType::n_NE, children);
                break;
            }
            default: break;
        }
    }
    return result;
}

Node Parser::arith_expr() {
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
    if (oldToken.name == "IF") return if_expr();
    else if (oldToken.name == "FOR") return for_expr();
    else if (oldToken.name == "WHILE") return while_expr();
    return Node(NodeType::n_NULL);
}

Node Parser::if_expr() {
    Token oldToken = currentToken;
    if (oldToken.type == TokenType::t_KEYWORD && oldToken.name == "IF") {
        advance();
        Node comparison = expr();
        Node trueExpr = Node(NodeType::n_NULL);
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "THEN") {
            advance();
            trueExpr = expr();
        } else {
            raiseError();
        }
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ELSE") {
            advance();
            Node falseExpr = expr();
            if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ENDIF") {
                std::vector<Node> children = {comparison, trueExpr, falseExpr};
                advance();
                return Node(NodeType::n_IF_ELSE, children);
            }
        } else if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ENDIF") {
            std::vector<Node> children = {comparison, trueExpr};
            advance();
            return Node(NodeType::n_IF, children);
        }
    }
    raiseError(); // nothing
}
Node Parser::for_expr() {
    Token oldToken = currentToken;
    if (oldToken.type == TokenType::t_KEYWORD && oldToken.name == "FOR") {
        advance();
        Node assignment = expr();
        Node end = Node(NodeType::n_NULL);
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "TO") {
            advance();
            end = expr();
        } else {
            raiseError();
        }
        // advance();
        Node commands = expr();
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "NEXT") {
            advance();
            Node step = expr();
            std::vector<Node> children = {assignment, end, commands, step};
            advance();
            return Node(NodeType::n_FOR, children);
        }
    }
    raiseError(); // nothing
}
Node Parser::while_expr() {
    Token oldToken = currentToken;
    if (oldToken.type == TokenType::t_KEYWORD && oldToken.name == "WHILE") {
        advance();
        Node comparison = expr();
        Node commands = Node(NodeType::n_NULL);
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "DO") {
            advance();
            commands = expr();
        } else {
            raiseError();
        }
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ENDWHILE") {
            std::vector<Node> children = {comparison, commands};
            advance();
            return Node(NodeType::n_WHILE, children);
        }
    }
    raiseError(); // nothing
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