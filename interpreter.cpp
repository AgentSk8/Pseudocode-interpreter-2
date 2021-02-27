#include "parser.h"
#include "interpreter.h"
#include <math.h>

/* INIT NUM WITH FLOAT VALUE */
Number::Number(float Value) {
    value = Value;
}
/* INIT NUM WITH STRING (ERROR) */
Number::Number(std::string msg) {
    throw std::runtime_error(msg);
}
/* OPERATOR "<<" OVERLOAD FOR NUM JUST OUTPUTS VALUE */
std::ostream &operator<<(std::ostream &os, Number const &n) {
    return os << n.value;
}

/* RECURSIVELY CALL VISIT ON EACH NODE AND OPERATE WITH SAID VALUES*/
Number Interpreter::visit(Node node) {
    // Switch the node type
    switch (node.type) {
        case NodeType::n_NUMBER:
            // just return a number if number
            return Number(node.value);
        case NodeType::n_ADD:
            // visit each node and add result
            return Number(visit(node.nodes[0]).value + visit(node.nodes[1]).value);
        case NodeType::n_SUBTRACT:
            // visit each node and sub. result
            return Number(visit(node.nodes[0]).value - visit(node.nodes[1]).value);
        case NodeType::n_MULTIPLY: 
            // visit each node and mult. result
            return Number(visit(node.nodes[0]).value * visit(node.nodes[1]).value);
        case NodeType::n_DIVIDE:
            // visit each node and div. result
            {
                // brackets put in so that we don't accidentally use n1 anywhere else (jump to case label error)
                Number n1 = visit(node.nodes[0]).value;
                // if denominator is zero raise zero error
                if (n1.value == 0)
                    throw std::runtime_error("Math error: Attempted to divide by zero\n");
                return Number(visit(node.nodes[0]).value / visit(node.nodes[1]).value);
            }
        case NodeType::n_POWER:
            return Number(pow(visit(node.nodes[0]).value, visit(node.nodes[1]).value));
        case NodeType::n_PLUS:
            // return a number with the node inside parens (or num) value
            return Number(visit(node.nodes[0]).value);
        case NodeType::n_MINUS:
            // return a number with the node inside parens (or num) negative value 
            return Number(-visit(node.nodes[0]).value);
        default:
            return Number("Runtime error.");

    };

}