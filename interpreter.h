#ifndef INTERPRETER_H
#define INTERPRETER_H

/* NUMBER STRUCT */
struct Number {
    float value; // holds a float value
    Number(float Value);
    Number(std::string msg);
};

/* NUMBER OPERATOR "<<" OVERLOAD */
std::ostream &operator<<(std::ostream &os, Number const &n);

/* INTERPRETER CLASS */
class Interpreter {
    public:
        Number visit(Node node); // visit each node and return result in parent node
};

#endif