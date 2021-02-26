/* NODE DATACLASS */
//---------------//
/**
* 1. init node with child nodes
* 2. init value node
*/
struct Node {
    NodeType type;
    float value;
    std::vector<Node> nodes;

    Node(NodeType Type, std::vector<Node> Nodes){
        type = Type;
        nodes = Nodes;
    }
    Node(NodeType Type, float Value){
        type = Type;
        value = Value
    }
    Node(NodeType Type){
        type = Type;
    }
};

/* NODE OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Node const &n) {
    if (n.type == NodeType::NUL) return os << " "; // space if none
    else if (n.type == NodeType::NUMBER) return os << n.value; // value if number
    else {
        NodeType t = n.type;
        using namespace NodeType; // simplify if else statements
        // NOTE: probably a better way to write these statements
        if (t == ADD) return os << "(" << n.nodes[0] << "+" << n.nodes[1] << ")";
        else if (t == SUBTRACT) return os << "(" << n.nodes[0] << "-" << n.nodes[1] << ")";
        else if (t == MULTIPLY) return os << "(" << n.nodes[0] << "*" << n.nodes[1] << ")";
        else if (t == DIVIDE) return os << "(" << n.nodes[0] << "/" << n.nodes[1] << ")";
        else if (t == PLUS) return os << "(+" << n.nodes[0] << ")";
        else if (t == MINUS) return os << "(-" << n.nodes[0] << ")";
        else return os << "?" // otherwise return unknown
    }
};

/* PARSER CLASS */
// see grammar.txt to understand the rules. functions will be 
// added/updated in the future to account for different orders
class Parser {
    public:

        // NOTE: could use iterators
        std::vector<Token> tokens;
        Token currentToken = Token(TokenType::NONE);
        int cursorPos;

        Parser(std::vector<Token> Tokens){
            tokens = Tokens;
            cursorPos = 0;
            currentToken = tokens[0];
        }
        Node parse(){
            if (currentToken.type == TokenType::NONE) return Node(NodeType::NUL);

            Node result = expr(); // call least priority (expr +/-)

            // If token is left unparsed, raise syntax error
            if (currentToken.type != TokenType::NONE) raiseError(); 

            return result; // return parent / root node
        }
    private:
        Node expr() {
            Node result = term(); // next top priority (term */÷)

            std::vector<TokenType> pm = {TokenType::PLUS, TokenType::MINUS}; // placeholder for t types

            while (std::find(pm.begin(), pm.end(), currentToken.type) != pm.end()) { // so while current token == plus or minus
                if (currentToken.type == TokenType::PLUS) {
                    advance();

                    std::vector<Node> children = {result, term()}; // child nodes of add node
                    result = Node(NodeType::ADD, children);
                } else if (currentToken.type == TokenType::MINUS) {
                    advance();

                    std::vector<Node> children = {result, term()}; // child nodes of subtract node
                    result = Node(NodeType::SUBTRACT, children);
                }
            }
            return result;
        }
        Node term() {
            Node result = factor(); // next (and final) top priority (factor number/parens)
            std::vector<TokenType> dm = {TokenType::MULTIPLY, TokenType::DIVIDE};
            while (std::find(dm.begin, dm.end(), currentToken.type) != dm.end()) {
                if (currentToken.type == TokenType::MULTIPLY) {
                    advance();
                    std::vector<Node> children = {result, factor()}; // child nodes of multiply node
                    result = Node(NodeType::MULTIPLY, children);
                } else if (currentToken.type == TokenType::DIVIDE) {
                    advance();
                    std::vector<Node> children = {result, factor()}; // child nodes of divide node
                    result = Node(result, factor());
                }
            }
            return result;
        }
        Node factor() {
            Token oldToken = currentToken; // need to create a copy because we advance later

            if (token.type == TokenType::LPAREN) {
                advance(); // advance into expression inside paren
                Node result = expr(); // evaluate this as a new expression
                if (currentToken.type != TokenType::RPAREN) {
                    raiseError(); // raise syntax error because parentheses haven't been closed
                }
                advance();
                return result;
            } else if (token.type == TokenType::NUMBER) {
                advance();
                return Node(NodeType::NUMBER, token.value); // return node of type number with the token's value
            } else if (token.type == TokenType::PLUS) {
                advance();
                std::vector<Node> plusValue = {factor()}; // call factor as it will either be +number or +() parentheses
                return Node(NodeType::PLUS, plusValue);
            } else if (token.type == TokenType::MINUS) {
                advance();
                std::vector<Node> minusValue = {factor()} // call factor as it will either be -number or -() parentheses
                return Node(NodeType::MINUS, minusValue);
            }
            raiseError(); // not any of the token types or invalid grammar
        }
        void raiseError() {
            throw std::runtime_error("Invalid Syntax.")
        }
        void advance() {
            // NOTE: this would also have to be updated to use iterators
            cursorPos++;
            if (cursorPos < tokens.size()) currentToken = tokens[cursorPos];
            else currentToken = Token(TokenType::NONE);
        }
};