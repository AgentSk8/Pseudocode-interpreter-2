#include "parser.h"
#include "interpreter.h"
#include <math.h>

/* INIT NUM WITH FLOAT VALUE */
Number::Number(float Value) {
    value = Value;
}

/* INIT NUM WITH STRING (ERROR) */
Number::Number(std::string msg) {
    value = -1;
    throw std::runtime_error(msg);
}

/* OPERATOR "<<" OVERLOAD FOR NUM JUST OUTPUTS VALUE */
std::ostream &operator<<(std::ostream &os, Number const &n) {
    return os << n.value;
}

/* CHILD SYMBOL TABLE WITH GLOBAL PARENT */
SymbolTable::SymbolTable(SymbolTable* Parent) {
    // NOTE: since pointer, use * to get value, & to get address and -> to get properties
    parent = Parent;
}

/* GLOBAL SYMBOL TABLE WITH NO PARENT*/
SymbolTable::SymbolTable(bool nul) {
    null = nul;
}

/* SYMBOL TABLE OF NULL VALUE*/
SymbolTable::SymbolTable() {
    SymbolTable tmp = SymbolTable(true);
    parent = &tmp;
}

/* DELETE VAR */
void SymbolTable::remove(std::string key) {
    if (includes(key)) m.erase(m.find(key));
}

/* IF KEY IN SYMBOL TABLE */
bool SymbolTable::includes(std::string key) {
    return m.find(key) != m.end();
}

/* SET KEY TO VALUE IN SYMBOL TABLE */
void SymbolTable::set(std::string key, Variable value) {
    remove(key); // redefinitions
    m.insert(std::pair<std::string,Variable>(key,value));
}

/* SET KEY TO FLOAT VALUE IN SYMBOL TABLE */
void SymbolTable::set(std::string key, float value) {
    remove(key); // redefinitions
    m.insert(std::pair<std::string,Variable>(key,Variable(Number(value))));
}

/* SYMBOL TABLE "<<" OPERATOR - PRINTS INTERIOR MAP*/
std::ostream &operator<<(std::ostream &os, SymbolTable const &st) {
    for(std::map<std::string,Variable>::const_iterator it = st.m.begin(); it != st.m.end(); ++it) {
       os << it->first << " " << it->second << "\n";
    }
    return os;
}

/* RETURN VARIABLE FROM THIS (OR PARENTS) SYMBOL TABLE*/
Variable SymbolTable::get(std::string key) {
    if (includes(key)) { // If key in this symbol table
        Variable tmp = m.find(key) -> second;
        return tmp; // Return value
    } else {
        if (parent -> null) { // If there is no parent
            return Variable(Number("Unexpected identifier" + key)); // Throw an exception
        }
        return parent -> get(key); // Otherwise try to get from parent
    }
}

/* FOR NUM TYPE VARS */
Variable::Variable(Number number_) {
    number = number_;
    value = number.value;
}

/* SYMBOL TABLE "<<" OPERATOR - PRINTS INTERIOR MAP*/
std::ostream &operator<<(std::ostream &os, Variable const &v) {
    return os << v.number;
}

/* RECURSIVELY CALL VISIT ON EACH NODE AND OPERATE WITH SAID VALUES*/
Variable Interpreter::visit(Node node) {
    // Switch the node type
    switch (node.type) {
        case NodeType::n_NUMBER:
            // just return a number if number
            return Variable(Number(node.value));
        case NodeType::n_ADD:
            // visit each node and add result
            return Variable(Number(visit(node.nodes[0]).value + visit(node.nodes[1]).value));
        case NodeType::n_SUBTRACT:
            // visit each node and sub. result
            return Variable(Number(visit(node.nodes[0]).value - visit(node.nodes[1]).value));
        case NodeType::n_MULTIPLY: 
            // visit each node and mult. result
            return Variable(Number(visit(node.nodes[0]).value * visit(node.nodes[1]).value));
        case NodeType::n_DIVIDE:
            // visit each node and div. result
            {
                // brackets put in so that we don't accidentally use n1 anywhere else (jump to case label error)
                Number n1 = visit(node.nodes[0]).value;
                // if denominator is zero raise zero error
                if (n1.value == 0)
                    throw std::runtime_error("Math error: Attempted to divide by zero\n");
                return Variable(Number(visit(node.nodes[0]).value / visit(node.nodes[1]).value));
            }
        case NodeType::n_POWER:
            return Variable(Number(pow(visit(node.nodes[0]).value, visit(node.nodes[1]).value)));
        case NodeType::n_PLUS:
            // return a number with the node inside parens (or num) value
            return Variable(Number(visit(node.nodes[0]).value));
        case NodeType::n_MINUS:
            // return a number with the node inside parens (or num) negative value 
            return Variable(Number(-visit(node.nodes[0]).value));
        case NodeType::n_VAR_ASSIGN:
            {
                Variable tmp = Variable(Number(visit(node.nodes[0]).value));
                globalSymbolTable.set(node.name, tmp);
                return tmp;
            }
        case NodeType::n_VAR_ACCESS:
            return globalSymbolTable.get(node.name);
        case NodeType::n_EE:
            return Variable(Number(visit(node.nodes[0]).value == visit(node.nodes[1]).value));
        case NodeType::n_NE:
            return Variable(Number(visit(node.nodes[0]).value != visit(node.nodes[1]).value));
        case NodeType::n_LT:
            return Variable(Number(visit(node.nodes[0]).value < visit(node.nodes[1]).value));
        case NodeType::n_GT:
            return Variable(Number(visit(node.nodes[0]).value > visit(node.nodes[1]).value));
        case NodeType::n_LTE:
            return Variable(Number(visit(node.nodes[0]).value <= visit(node.nodes[1]).value));
        case NodeType::n_GTE:
            return Variable(Number(visit(node.nodes[0]).value >= visit(node.nodes[1]).value));
        case NodeType::n_AND:
            return Variable(Number(visit(node.nodes[0]).value and visit(node.nodes[1]).value));
        case NodeType::n_OR:
            return Variable(Number(visit(node.nodes[0]).value or visit(node.nodes[1]).value));
        case NodeType::n_NOT:
            return Variable(Number(not visit(node.nodes[0]).value));
        case NodeType::n_IF: {
            bool result = visit(node.nodes[0]).value; // get the condition
            if (result) return Variable(Number(visit(node.nodes[1]).value));
            return Variable(Number(0));
        }
        case NodeType::n_IF_ELSE: {
            bool result = visit(node.nodes[0]).value;
            if (result) return Variable(Number(visit(node.nodes[1]).value));
            return Variable(Number(visit(node.nodes[2]).value));
        }
        default:
            return Variable(Number("Runtime error."));

    };

}