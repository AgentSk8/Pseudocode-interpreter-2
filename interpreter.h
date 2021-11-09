#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>

/* NUMBER STRUCT */
struct Number {
    float value; // holds a float value
    bool nret; // return val?
    Number(float Value);
    Number(std::string msg);
    Number();
};

/* STRING STRUCT */
struct String {
    std::string value;
    String(std::string Value);
    String(char Value);
    String();
};

/* Forward declar Variable for symbol table and list */
struct Variable;

// symbol table declared here for function
struct SymbolTable {
    public:
    std::map<std::string, Variable> m;
    SymbolTable* parent;
    bool null;

    SymbolTable(SymbolTable* Parent);
    SymbolTable();
    SymbolTable(bool nul);
    bool includes(std::string key);
    void set(std::string key, Variable value);
    void set(std::string key, float value);
    void remove(std::string key);
    Variable get(std::string key);
};

/* FUNCTION STRUCT */
struct Function {
    Node code;
    SymbolTable smbt;
    // code has structure
    // |_ name
    // |_ children
    //    |_ 0 - args
    //    |_ 1 - execution - return statement auto returns
    std::string name;
    Function(Node Code); // PASS POINTER TO THE NODE, CREATE COPY
    Function(); // Empty for declaration in Variable
};

/* LIST STRUCT */
struct List {
    std::vector<Variable> values;
    List(std::vector<Variable> Values);
    List();
};

/* VARIABLE STRUCT */
// NOTE: there is probably a better way to
// do what I am doing here using polymorphism
// but this works. ¯\_(ツ)_/¯
// Parent class needed so only one type can
// be returned - add layer of abstraction
struct Variable {
    /* BASE TYPES - DEFINED DEPENDS ON TYPE*/
    Number number;
    String string;
    List list;
    Function function;

    /* QUICK ACCESS TO NUMBER'S VALUE */
    float value;
    /* SO WE KNOW WHETHER TO ACCESS this.number, string...*/
    std::string type;

    /* EMPTY VAR */
    Variable();

    /* DECLARATION OF EACH OF THE TYPES*/
    Variable(Number number_); // for int / float values
    Variable(String string_);
    Variable(List list_);
    Variable(Function function_);

    /* CAN ALSO BE DEFINED BY THEIR SUBLIMINAL (?) TYPES */
    Variable(float number_);
    Variable(std::string string_);
    Variable(std::vector<Variable> list_);
    // no subliminal for function... as node is not specifically a function
};

/* NUMBER OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Number const &n);

/* STRING OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, String const &s);

/* LIST OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, List const &l);

/* FUNCTION OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Function const &f);

/* SYMBOLTABLE OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, SymbolTable const &st);

/* VARIABLE OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Variable const &v);

/* EQUALITY OPERATOR OVERLOADS */
bool operator!=(Variable const &v1,Variable const &v2);
bool operator==(Variable const &v1,Variable const &v2);
bool operator==(Number const &n1,Number const &n2);
bool operator==(String const &s1,String const &s2);
bool operator==(List const &l1,List const &l2);

/* INTERPRETER CLASS */
class Interpreter {
    SymbolTable& globalSymbolTable;
    bool return_ = 0;
    Variable return_value;
    public:
        Interpreter(SymbolTable& GlobalSymbolTable) : globalSymbolTable(GlobalSymbolTable) {}
        Variable visit(Node node); // visit each node and return result in parent node
};

#endif