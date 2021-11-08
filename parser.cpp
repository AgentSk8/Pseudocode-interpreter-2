#include "parser.h"
#include "lexer.h" // for Token
#include "error.h"
#include <algorithm>
#include <sstream>
#include <ostream>

/* ERROR DEFAULT */
Error E_Parser = Error(e_InvalidSyntax);

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
        else if (t == NodeType::n_PRINT) return os << "{PRINT:" << n.nodes[0] << "}";
        else if (t == NodeType::n_READ) return os << "{READ:" << n.name << "}";
        else if (t == NodeType::n_INPUT) return os << "{INPUT:" << n.name << "}";
        else if (t == NodeType::n_IF_ELSE) return os << "(" << n.nodes[0] << "?" << n.nodes[1] << ":" << n.nodes[2] << ")"; 
        else if (t == NodeType::n_FOR) return os << "(for(" << n.nodes[0] << "->" << n.nodes[1] << ")::(" << n.nodes[2] << ")" << "next)";
        else if (t == NodeType::n_WHILE) return os << "(while(" << n.nodes[0] << ")::(" << n.nodes[1] << ")" << "next)";
        else if (t == NodeType::n_STRING) return os << '"' << n.name << '"';
        else if (t == NodeType::n_LIST || t == NodeType::n_BLOCK) {
            os << "([";
            for (long long i = 0; i < n.nodes.size(); i++) {
                os << n.nodes[i];
                if (i != n.nodes.size()-1) os << ", ";
            }
            os << "])";
            return os;
        }
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
    Node result = Node(NodeType::n_BLOCK); // return a block of code
    while (currentToken.type != TokenType::t_NONE) { // expr until equal to none
        Node tmp_result = expr(); // call least priority (expr +/-)
        result.nodes.push_back(tmp_result);
        // advance all of the separators / newlines

        int c = sep_expr();
        if (c == 0) {
            if (currentToken.type == TokenType::t_NONE) break;
            else {
                raiseError("Expected ';', newline or EOF not '" + currentToken.to_string() + "'."); // no separators and not EOF
            }
        }
    }

    // If token is left unparsed, raise syntax error
    //if (currentToken.type != TokenType::t_NONE) raiseError(); 

    return result; // return parent / root block node (node of nodes)
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
            raiseError("Unclosed parentheses!"); // raise syntax error because parentheses haven't been closed
        }
        advance();
        return result;
    } else if (oldToken.type == TokenType::t_NUMBER) {
        advance();
        return Node(NodeType::n_NUMBER, oldToken.value); // return node of type number with the token's value
    } else if (oldToken.type == TokenType::t_STRING) {
        advance();
        return Node(NodeType::n_STRING, oldToken.name);
    } else if (oldToken.type == TokenType::t_IDENTIFIER) { // access the identifier
        advance();
        return Node(NodeType::n_VAR_ACCESS, oldToken.name);
    }
    if (oldToken.type == TokenType::t_LSQBRACKET) return list_expr();
    else if (oldToken.name == "IF") return if_expr();
    else if (oldToken.name == "FOR") return for_expr();
    else if (oldToken.name == "WHILE") return while_expr();
    return builtin_expr();
}

Node Parser::list_expr() {
    Token oldToken = currentToken;
    std::vector<Node> elements;
    while (currentToken.type != TokenType::t_RSQBRACKET) {
        advance();
        Node element = expr();
        elements.push_back(element);
    }
    advance();
    return Node(NodeType::n_LIST, elements);
}

Node Parser::if_expr() {
    Token oldToken = currentToken;
    if (oldToken.type == TokenType::t_KEYWORD && oldToken.name == "IF") {
        advance(); // past IF
        Node comparison = expr();
        sep_expr();
        Node trueExpr = Node(NodeType::n_BLOCK);
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "THEN") {
            advance(); // advance past 'THEN'
            sep_expr();
            // whilst we haven't reached end of truexpr (endif or else),
            // keep adding to the block expr that is truexpr
            /*while (currentToken.name != "ENDIF" && currentToken.name != "ELSE") {
                Node tmpExpr = expr();
                trueExpr.nodes.push_back(tmpExpr); // add this expr to the block expr node
                while (currentToken.type == TokenType::t_SEP || currentToken.type == TokenType::t_NL) advance(); // advance past all separators
            }*/
            trueExpr = block_expr({ "ELSE","ENDIF" }); // block that runs if IF is true ends with ELSE or ENDIF
        } else {
            raiseError("Expected 'THEN' keyword after 'IF', not '"+currentToken.to_string()+"'");
        }
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ELSE") {
            advance(); // past 'ELSE'
            sep_expr();
            Node falseExpr = block_expr({ "ENDIF" });
            if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ENDIF") {
                std::vector<Node> children = {comparison, trueExpr, falseExpr};
                advance(); // past ENDIF
                return Node(NodeType::n_IF_ELSE, children);
            }
        } else if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ENDIF") {
            std::vector<Node> children = {comparison, trueExpr};
            advance();
            return Node(NodeType::n_IF, children);
        }
    }
    raiseError(); // nothing
    return Node(NodeType::n_NULL);
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
            raiseError("Expected 'TO' keyword after 'FOR', not '"+currentToken.to_string()+"'");
        }
        // BLOCK:

        Node commands = block_expr({ "NEXT" });

        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "NEXT") {
            advance();
            Node step = expr();
            std::vector<Node> children = {assignment, end, commands, step};
            advance();
            return Node(NodeType::n_FOR, children);
        }
    }
    raiseError(); // nothing
    return Node(NodeType::n_NULL);
}
Node Parser::while_expr() {
    Token oldToken = currentToken;
    if (oldToken.type == TokenType::t_KEYWORD && oldToken.name == "WHILE") {
        advance();
        Node comparison = expr();
        Node commands = Node(NodeType::n_NULL);
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "DO") {
            advance();
            commands = block_expr({ "ENDWHILE" });
        } else {
            raiseError("Expected 'DO' keyword after 'WHILE', not '"+currentToken.to_string()+"'");
        }
        if (currentToken.type == TokenType::t_KEYWORD && currentToken.name == "ENDWHILE") {
            std::vector<Node> children = {comparison, commands};
            advance();
            return Node(NodeType::n_WHILE, children);
        }
    }
    raiseError(); // nothing
    return Node(NodeType::n_NULL);
}

Node Parser::builtin_expr() {
    Token oldToken = currentToken;
    // TODO: commas
    if (oldToken.type == TokenType::t_KEYWORD) {
        std::string n = oldToken.name;
        advance();
        if (n == "PRINT") {
            std::vector<Node> children = { expr() };
            return Node(NodeType::n_PRINT, children);
        } else if (n == "READ") {
            if (currentToken.type == TokenType::t_IDENTIFIER) {
                std::string id = currentToken.name;
                advance();
                return Node(NodeType::n_READ, id);
            }
        } else if (n == "INPUT") {
            if (currentToken.type == TokenType::t_IDENTIFIER) {
                std::string id = currentToken.name;
                advance();
                return Node(NodeType::n_INPUT, id);
            }
        } else {
            raiseError("Invalid keyword: '"+currentToken.name+"'");
        }
    } else {
        raiseError("Expected (END-*?) keyword!");
    }
    return Node(NodeType::n_NULL);
}

Node Parser::block_expr(std::vector<std::string> terminators) { // adds all exprs until terminator KEYWORD reached (e.g. ELSE/ENDIF for truexpr in IF)
    Node ret = Node(NodeType::n_BLOCK);
    //while (currentToken.name != "ENDIF" && currentToken.name != "ELSE") {
    while (std::find(terminators.begin(),terminators.end(),currentToken.name) == terminators.end()) {
        Node tmpExpr = expr();
        ret.nodes.push_back(tmpExpr); // add this expr to the block expr node
        sep_expr();
    }
    return ret;
}

int Parser::sep_expr() {
    int c = 0;
    while (currentToken.type == TokenType::t_SEP || currentToken.type == TokenType::t_NL) {
        advance(); // advance past all separators
        c++;
    }
    return c;
}

void Parser::raiseError(std::string msg) {
    E_Parser.throw_(msg);
    
}

void Parser::raiseError() {
    E_Parser.throw_("Unknown Syntax error!");    
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