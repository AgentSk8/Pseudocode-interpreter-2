#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>

/* NUMBER STRUCT */
struct Number {
    float value; // holds a float value
    Number(float Value);
    Number(std::string msg);
};

struct Variable {
    Number number = Number(-1);
    float value;
    // TODO: other var types
    Variable(Number number_); // for int / float values
};

struct SymbolTable {
    std::map<std::string, Variable> m;

    SymbolTable();
    bool includes(std::string key);
    void set(std::string key, Variable value);
    Variable get(std::string key);
}

/* NUMBER OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Number const &n);

/* INTERPRETER CLASS */
class Interpreter {
    public:
        Variable visit(Node node); // visit each node and return result in parent node
};

#endif