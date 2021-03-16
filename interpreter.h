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
    SymbolTable* parent;
    bool null;

    SymbolTable(SymbolTable* Parent);
    SymbolTable();
    SymbolTable(bool nul);
    bool includes(std::string key);
    void set(std::string key, Variable value);
    void remove(std::string key);
    Variable get(std::string key);
};

/* NUMBER OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Number const &n);

/* SYMBOLTABLE OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, SymbolTable const &st);

/* VARIABLE OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Variable const &v);

/* INTERPRETER CLASS */
class Interpreter {
    SymbolTable& globalSymbolTable;
    public:
        Interpreter(SymbolTable& GlobalSymbolTable) : globalSymbolTable(GlobalSymbolTable) {}
        Variable visit(Node node); // visit each node and return result in parent node
};

#endif