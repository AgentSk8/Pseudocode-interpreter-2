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

/* NO RETURN */
Number::Number() {
    nret = 1;
}

/* STRING INIT */
String::String(std::string Value) {
    value = Value;
}
/* NULL STRING */
String::String() {
    value = "";
}

/* OPERATOR "<<" OVERLOAD FOR NUM JUST OUTPUTS VALUE */
std::ostream &operator<<(std::ostream &os, Number const &n) {
    if (n.nret) return os;
    return os << n.value;
}

/* OPERATOR "<<" OVERLOAD FOR NUM JUST OUTPUTS VALUE */
std::ostream &operator<<(std::ostream &os, String const &s) {
    return os << s.value;
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
    type = "number";
}

Variable::Variable(String string_) {
    string = string_;
    type = "string";
}

Variable::Variable(std::string string_) {
    string = String(string_);
    type = "string";
}

Variable::Variable(float number_) {
    number = Number(number_);
    value = number.value;
    type = "number";
}
/* VARIABLE << OPERATOR */
std::ostream &operator<<(std::ostream &os, Variable const &v) {
    if (v.type == "number") {
        return os << v.number;
    }
    return os << v.string;
}

/* RECURSIVELY CALL VISIT ON EACH NODE AND OPERATE WITH SAID VALUES*/
Variable Interpreter::visit(Node node) {
    // Switch the node type
    switch (node.type) {
        case NodeType::n_NUMBER:
            // just return a number if number
            return Variable(Number(node.value));
        case NodeType::n_STRING:
            // return string var
            return Variable(String(node.name));
        case NodeType::n_ADD: {
            // visit each node and add result
            Variable node1 = visit(node.nodes[0]);
            Variable node2 = visit(node.nodes[1]);
            if (node1.type == "number" && node2.type == "number")
                return Variable(node1.value + node2.value);
            else if (node1.type == "number") {
                std::string n1 = std::to_string(node1.value);
                n1.erase(n1.find_last_not_of('0')+1,std::string::npos);
                if (n1.back() == '.') n1.erase(std::prev(n1.end()));
                return Variable(n1 + node2.string.value);
            } else if (node2.type == "number") {
                std::string n2 = std::to_string(node2.value);
                n2.erase ( n2.find_last_not_of('0') + 1, std::string::npos );
                if (n2.back() == '.') n2.erase(std::prev(n2.end()));
                return Variable(node1.string.value + n2);
            } else {
                return Variable(node1.string.value + node2.string.value);
            }
        }
        case NodeType::n_SUBTRACT:
            // visit each node and sub. result
            return Variable(visit(node.nodes[0]).value - visit(node.nodes[1]).value);
        case NodeType::n_MULTIPLY: {
            Variable node1 = visit(node.nodes[0]);
            Variable node2 = visit(node.nodes[1]);
            if (node1.type == "number" && node2.type == "number")
                return Variable(node1.value * node2.value);
            else {
                std::string ret;
                if (node1.type == "number") {
                    ret = "";
                    for (int i = 0; i < node1.value; i++) {
                        ret += node2.string.value;
                    }
                } else {
                    ret = "";
                    for (int i = 0; i < node2.value; i++) {
                        ret += node1.string.value;
                    }
                }
                return Variable(ret);
            }
        }
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
                Variable cnode = visit(node.nodes[0]);
                Variable tmp = Variable(0);
                if (cnode.type == "number")
                    tmp = Variable(cnode.value);
                else
                    tmp = Variable(cnode.string);
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
        case NodeType::n_FOR: {
            // assignment, end, commands, step
            visit(node.nodes[0]);
            while (globalSymbolTable.get(node.nodes[0].name).value != visit(node.nodes[1]).value) {
                visit(node.nodes[2]);
                globalSymbolTable.set(node.nodes[0].name,visit(node.nodes[3]));
            }
            return Variable(Number());
        }
        case NodeType::n_WHILE: {
            // comparison, commands
            while (visit(node.nodes[0]).value) {
                visit(node.nodes[1]);
            }
            return Variable(Number());
        }
        case NodeType::n_PRINT: std::cout << visit(node.nodes[0]) << std::endl; return Variable(Number());
        case NodeType::n_READ: {
            std::string inp;
            std::getline(std::cin, inp);
            globalSymbolTable.set(node.name, Variable(inp));
            return Variable(Number());
        }
        case NodeType::n_INPUT: {
            float inp;
            std::cin >> inp;
            globalSymbolTable.set(node.name, inp);
            return Variable(Number());
        }
        default:
            return Variable(Number("Runtime error."));

    };
}