#include "file.h"
#include "parser.h"
#include "interpreter.h"
#include "error.h"
#include <math.h>
#include <algorithm>

/* ERROR DEFAULT */
Error E_Interpreter = 	Error(e_RuntimeError);
Error E_Math = 			Error(e_MathError);
Error E_Module = 		Error(e_ModuleNotFoundError);

/* HELPER FUNCTION TO REMOVE ALL SUBSTRINGS FROM A MAIN STRING */
std::string substringRemove(std::string main_string, std::string substring) {
    std::string new_string;
    if (main_string.size() < substring.size()) return main_string;// if sub is larger than main, it can't be substring
    else if (main_string.size() == substring.size()) { // if they are equal size, then either main==sub, or sub can't be a substring of main
        if (main_string == substring) return "";
        else return main_string;
    }
    long long i = 0;
    while (i < main_string.size()) {
        if (main_string.size()-i>= substring.size() && main_string[i] == substring[0]) { // if matches first char if () / if remaining chars are longer than substring then there won't be a substring
            long long j = 1;
            while (j < substring.size()) {// check all other characters
                if (substring[j] != main_string[i+j]) break; // if those don't match, break
                j++;
            }
            if (j == substring.size()) i+=substring.size()-1; // if that IS  a substring, advance its size to skip it.
            else new_string += main_string[i];// add the initial char that wasn't part of the substring back.
        } else {
            new_string += main_string[i]; // otherwise it is not part of substring so can be added to new string
        }
        i++;
    }
    return new_string;
}

/* HELPER FUNCTION TO MAKE STRING UPPER */
std::string s_toupper(std::string s) {
    // iterator first1, last1, first2, unary operator applies toupper to each char.
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c) {return std::toupper(c);});
    return s;
}

/* INIT NUM WITH FLOAT VALUE */
Number::Number(float Value) {
    value = Value;
    nret = 0;
}

/* INIT NUM WITH STRING (ERROR) */
Number::Number(std::string msg) {
    value = -1;
    E_Interpreter.throw_(msg);
}

/* NO RETURN */
Number::Number() {
    nret = 1;
}

/* STRING INIT */
String::String(std::string Value) {
    value = Value;
}
/* CHAR STRING INIT */
String::String(char Value) {
    std::string tmp(1,Value);
    value = tmp;
}
/* NULL STRING */
String::String() {
    value = "";
}

/*LIST INIT*/
List::List(std::vector<Variable> Values) {
    values = Values;
}

/*EMPTY LIST*/
List::List() {}

/* FUNCTION DEFINITION. SEE HEADER FILE FOR CODE STRUCTURE */
Function::Function(Node Code) {
    code = Code;
    name = code.name;
    argus = code.nodes[0].nodes;
    for (int i = 0; i < argus.size(); i++) {
        smbt.set(argus[i].name,0); // set said key at undefined 0 for now
    }
}

Function::Function() {}; // Empty for declaration in Variable

/* OPERATOR "<<" OVERLOAD FOR NUM JUST OUTPUTS VALUE */
std::ostream &operator<<(std::ostream &os, Number const &n) {
    if (n.nret) return os;
    return os << n.value;
}

/* OPERATOR "<<" OVERLOAD FOR STR JUST OUTPUTS VALUE */
std::ostream &operator<<(std::ostream &os, String const &s) {
    return os << s.value;
}

/* OPERATOR "<<" OVERLOAD FOR LIST JUST OUTPUTS ELEMENTS */
std::ostream &operator<<(std::ostream &os, List const &l) {
    long long c = l.values.size();
    os << '[';
    for (long long i = 0; i < c; i++) {
        os << l.values[i];
        if (i != c-1) os << ", ";
    }
    return os << ']';
}

/* OPERATOR "<<" OVERLOAD FOR FUNCTION OUTPUTS NAME AND ADDRESS IN MEMORY */
std::ostream &operator<<(std::ostream &os, Function const &f) {
    return os << "<function " << f.name << " @ " << &f << ">\nWith symbols:\n" << f.smbt;

}

/* ==" OPERATOR OVERLOADS */
bool operator==(Variable const &v1,Variable const &v2) {
    if (v1.type != v2.type) return 0;
    if (v1.type == "number") return v1.number == v2.number;
    if (v1.type == "string") return v1.string == v2.string;
    if (v1.type == "list") return v1.list == v2.list;
    if (v1.type == "function") return v1.function == v2.function;
    throw std::runtime_error("Interpreter error: Invalid type for equality: " + v1.type);
}
bool operator!=(Variable const &v1,Variable const &v2) {
    return !(v1 == v2);
}
bool operator==(Number const &n1,Number const &n2) {
    return n1.value == n2.value;
}
bool operator==(String const &s1,String const &s2) {
    return s1.value == s2.value;
}
bool operator==(List const &l1,List const &l2) {
    return l1.values == l2.values;
}
bool operator==(Function const &f1,Function const &f2) {
    return f1.name == f2.name && f1.code.nodes == f2.code.nodes;
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

/* FOR EMPTY DECLARATIONS */
Variable::Variable() { }

/* FOR NUM TYPE VARS */
Variable::Variable(Number number_) {
    number = number_;
    value = number.value;
    type = "number";
}
/* STRING */
Variable::Variable(String string_) {
    string = string_;
    type = "string";
}
/* LIST */
Variable::Variable(List list_) {
    list = list_;
    type = "list";
}
/* FUNCTION */
Variable::Variable(Function function_) {
    function = function_;
    type = "function";
}
/* STRING */
Variable::Variable(std::string string_) {
    string = String(string_);
    type = "string";
}
/* NUMBER */
Variable::Variable(float number_) {
    number = Number(number_);
    value = number.value;
    type = "number";
}
/* LIST */
Variable::Variable(std::vector<Variable> list_) {
    list = List(list_);
    type = "list";
}




/* CODE FOR RUNNING SHELL COMMANDS AND GETTING OUTPUT. CREDIT GOES TO https://raymii.org/s/articles/Execute_a_command_and_get_both_output_and_exit_code.html*/
///////////////////////////////////
////////// STARTS HERE ////////////
///////////////////////////////////
struct CommandResult {
    std::string output;
    int exitstatus;

    friend std::ostream &operator<<(std::ostream &os, const CommandResult &result) {
        os << "command exitstatus: " << result.exitstatus << " output: " << result.output;
        return os;
    }
    bool operator==(const CommandResult &rhs) const {
        return output == rhs.output &&
               exitstatus == rhs.exitstatus;
    }
    bool operator!=(const CommandResult &rhs) const {
        return !(rhs == *this);
    }
};

class Command {

public:
    static CommandResult exec(const std::string &command) {
        int exitcode = 255;
        std::array<char, 1048576> buffer {};
        std::string result;
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define WEXITSTATUS
#endif
        FILE *pipe = popen(command.c_str(), "r");
        if (pipe == nullptr) {
            throw std::runtime_error("popen() failed!");
        }
        try {
            std::size_t bytesread;
            while ((bytesread = fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0) {
                result += std::string(buffer.data(), bytesread);
            }
        } catch (...) {
            pclose(pipe);
            throw;
        }
        exitcode = WEXITSTATUS(pclose(pipe));
        return CommandResult{result, exitcode};
    }
};
/////////////////////////////////
////////// ENDS HERE ////////////
/////////////////////////////////




/* VARIABLE << OPERATOR */
std::ostream &operator<<(std::ostream &os, Variable const &v) {
    if (v.type == "number") return os << v.number;
    else if (v.type == "string") return os << v.string;
    else if (v.type == "list") return os << v.list;
    else if (v.type == "function") return os << v.function;
    return os << "?";
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

            // NOTE: There is certainly a much better way to do this using oop (i.e. each class having an add member and Variable calling on its subclass add member passing another variable (pointer?)
            if (node1.type == "number") {
                /* NUMBER + NUMBER */
                if (node2.type == "number") return Variable(node1.value + node2.value);
                /* NUMBER + STRING -> ADDS NUMBER TO FRONT OF STRING */
                else if (node2.type == "string")  {
                    // string manipulation so integer float is repr as int (i.e. 5.0 -> 5)
                    std::string n1 = std::to_string(node1.value);
                    n1.erase(n1.find_last_not_of('0')+1,std::string::npos);
                    if (n1.back() == '.') n1.erase(std::prev(n1.end()));
                    return Variable(n1 + node2.string.value);
                /* NUMBER + LIST -> ADDS NUMBER TO FRONT OF LIST */
                } else if (node2.type == "list") {
                    std::vector<Variable> vals;
                    vals.push_back(node1);
                    for (long long i = 0; i < node2.list.values.size(); i++) {
                        vals.push_back(node2.list.values[i]);
                    }
                    return Variable(vals);
                }
            } else if (node1.type == "string") {
                /* STRING + NUMBER -> ADD NUMBER TO BACK OF STRING */
                if (node2.type == "number") {
                    // string manipulation so integer float is repr as int (i.e. 5.0 -> 5)
                    std::string n2 = std::to_string(node2.value);
                    n2.erase ( n2.find_last_not_of('0') + 1, std::string::npos );
                    if (n2.back() == '.') n2.erase(std::prev(n2.end()));
                    return Variable(node1.string.value + n2);
                /* STRING + STRING -> CONCATENATE STRINGS*/
                } else if (node2.type == "string") return Variable(node1.string.value + node2.string.value);
                /* STRING + LIST -> ADD STRING TO FRONT OF LIST */
                else if (node2.type == "list") {
                    std::vector<Variable> vals;
                    vals.push_back(node1);
                    for (long long i = 0; i < node2.list.values.size(); i++) {
                        vals.push_back(node2.list.values[i]);
                    }
                    return Variable(vals);
                }
            } else if (node1.type == "list") {
                /* LIST + NUMBER/STRING -> ADD NUMBER/STRING TO BACK */
                if (node2.type == "number" || node2.type == "string") {
                    std::vector<Variable> vals = node1.list.values;
                    vals.push_back(node2);
                    return Variable(vals);
                /* LIST + LIST -> adds elements from list to list (not list itself, unless element from list is list)*/
                } else if (node2.type == "list") {
                    std::vector<Variable> vals = node1.list.values;
                    for (long long i = 0; i < node2.list.values.size(); i++) {
                        vals.push_back(node2.list.values[i]);
                    }
                    return Variable(vals);
                }
            } else {
                return Variable(Number("Invalid left-side type for operation '+': " + node1.type));
            }
            return Variable(Number("Invalid right-side type for operation '+': " + node2.type));
        }
        case NodeType::n_SUBTRACT: {
            // visit each node and sub. result
            //return Variable(visit(node.nodes[0]).value - visit(node.nodes[1]).value);

            Variable node1 = visit(node.nodes[0]);
            Variable node2 = visit(node.nodes[1]);
            // NOTE: There is certainly a much better way to do this using oop (i.e. each class having a sub member and Variable calling on its subclass add member passing another variable (pointer?)
            if (node1.type == "number") {
                /* NUMBER - NUMBER */
                if (node2.type == "number") return Variable(node1.value - node2.value);
                /* NUMBER - STRING -> Error for now.. // Comment is: FOR EACH DIGIT IN NUMBER, REMOVE THAT DIGIT IF IT IS IN STRING AND RETURN A NUMBER */
                else if (node2.type == "string")  {
                    /*
                    // string manipulation so integer float is repr as int (i.e. 5.0 -> 5)
                    std::string n1 = std::to_string(node1.value);
                    n1.erase(n1.find_last_not_of('0')+1,std::string::npos);
                    if (n1.back() == '.') n1.erase(std::prev(n1.end()));
                    // loop through each char in second string
                    for (long long i = 0; i < n2.size(); i++) {
                        std::string nstring; // empty string to which add chars that != current char
                        char c1 = n2[i];
                        for (long long j = 0; j < n1.size(); j++){
                            char c2 = n1[j];
                            if (c1!=c2) nstring += c2; // add char from n1 that isn't in n2.
                        }
                        n1 = nstring; // set n1 to nstring so it has removed its chars
                    }
                    return Variable(std::stof(n1));*/
                    return Variable(Number("Invalid operation NUMBER - STRING"));
                /* NUMBER - LIST -> REMOVES NUMBER ELEMENTS FROM FRONT */
                } else if (node2.type == "list") {
                    std::vector<Variable> vals;
                    // loop all in range number -> length
                    for (long long i = node1.value; i < node2.list.values.size(); i++) {
                        vals.push_back(node2.list.values[i]);
                    }
                    return Variable(vals);
                }
            } else if (node1.type == "string") {
                /* STRING - NUMBER -> ERROR (for now...) */
                if (node2.type == "number") return Variable(Number("Invalid operation STRING - NUMBER"));
                /* STRING - STRING -> REMOVE SUBSTRINGS EQUAL TO STRING 2 FROM MAIN STRING OUT */
                else if (node2.type == "string") {
                    std::string n1_s = node1.string.value;
                    std::string n2_s = node2.string.value;
                    std::string new_string = substringRemove(n1_s, n2_s);
                    return Variable(new_string);
                /* STRING - LIST -> apply subtraction to string for every element in list. */
                } else if (node2.type == "list") {
                    std::string n1_s = node1.string.value;
                    for (long long i = 0; i < node2.list.values.size(); i++) {
                        if (node2.list.values[i].type != "string"){
                            std::string type = s_toupper(node2.list.values[i].type);
                            return Variable(Number("Invalid operation STRING - " + type ));
                        }
                        n1_s = substringRemove(n1_s,node2.list.values[i].string.value);
                    }
                    return Variable(n1_s); // return the new string
                }
            } else if (node1.type == "list") {
                /* LIST - NUMBER -> TAKES NUMBER ELEMENTS OUT FROM BACK */
                if (node2.type == "number") {
                    std::vector<Variable> vals;
                    // loop over all in range 0 -> length - number
                    for (long long i = 0; i < node1.list.values.size() - node2.value; i++) {
                        vals.push_back(node1.list.values[i]);
                    }
                    return Variable(vals);
                /* LIST - LIST -> REMOVE ANY ELEMENT FROM LIST2 THAT IS IN LIST1 */
                } else if (node2.type == "list") {
                    std::vector<Variable> vals;
                    for (long long i = 0; i < node1.list.values.size(); i++) {
                        bool in = false; // whether it is in list2.
                        for (long long j = 0; j < node2.list.values.size(); j++) {
                            if (node1.list.values[i] == node2.list.values[j]){
                                in = true;break;
                            }
                        }
                        if (!in) vals.push_back(node1.list.values[i]);
                    }
                    return Variable(vals);
                }
            } else {
                return Variable(Number("Invalid left-side type for operation '+': " + node1.type));
            }
            return Variable(Number("Invalid right-side type for operation '+': " + node2.type));
        }
        case NodeType::n_MULTIPLY: {
            Variable node1 = visit(node.nodes[0]);
            Variable node2 = visit(node.nodes[1]);
            if (node1.type == "number" && node2.type == "number")
                return Variable(node1.value * node2.value);
            else if (node1.type == "number" || node2.type == "number") {
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
            } else {
                return Variable(Number("Invalid operation " + s_toupper(node1.type) + " * " + s_toupper(node2.type)));
            }
        }
        case NodeType::n_DIVIDE: {
            Variable node1 = visit(node.nodes[0]);
            Variable node2 = visit(node.nodes[1]);
            if (node1.type == "number" && node2.type == "number") {
                // if denominator is zero raise zero error
                if (node2.value == 0)
                    E_Math.throw_("Division by zero error!");
                return Variable(node1.value/node2.value);
            } else if (node1.type == "string" && node2.type == "number") {
                std::string new_string;
                for (long long i = 0; i < node1.string.value.size(); i++) {
                    if (i != node2.value) new_string += node1.string.value[i];
                }
                return new_string;
            } else if (node1.type == "list" && node2.type == "number") {
                std::vector<Variable> vals;
                for (long long i = 0; i < node1.list.values.size(); i++) {
                    if (i != node2.value) vals.push_back(node1.list.values[i]);
                }
                return vals;
            }
            return Variable(Number("Invalid operation " + s_toupper(node1.type) + " ÷ " + s_toupper(node2.type)));
        }
        case NodeType::n_POWER: {
            Variable node1 = visit(node.nodes[0]);
            Variable node2 = visit(node.nodes[1]);
            if (node1.type == "number" && node2.type == "number") return Variable(Number(pow(node1.value,node2.value)));
            else if (node2.type == "number") {
                if (node1.type == "string") {
                    return Variable(String(node1.string.value[node2.value]));
                } else if (node1.type == "list") {
                    return node1.list.values[node2.value];
                }
            }
            return Variable(Number("Invalid operation " + s_toupper(node1.type) + " ^ " + s_toupper(node2.type)));
        }
        case NodeType::n_PLUS:
            // return a number with the node inside parens (or num) value
            return Variable(Number(visit(node.nodes[0]).value));
        case NodeType::n_MINUS:
            // return a number with the node inside parens (or num) negative value 
            return Variable(Number(-visit(node.nodes[0]).value));
        case NodeType::n_VAR_ASSIGN:
            {
                Variable cnode = visit(node.nodes[0]);
                globalSymbolTable.set(node.name, cnode);
                return cnode;
            }
        case NodeType::n_LIST: {
            std::vector<Variable> elements;
            for (long long i = 0; i < node.nodes.size(); i++) {
                elements.push_back(visit(node.nodes[i]));
            }
            return Variable(elements);
        }
        case NodeType::n_VAR_ACCESS:
            return globalSymbolTable.get(node.name);
        case NodeType::n_EE: {
			Variable n1 = visit(node.nodes[0]);
			Variable n2 = visit(node.nodes[1]);
			if (n1.type == "number"){
				if (n2.type == "number") {
					return Variable(Number(n1.value==n2.value));
				} else if (n2.type == "string") {
					return Variable(Number(std::to_string(n1.value)==n2.string));
				} else return Variable(Number(0));
			} else if (n1.type == "string") {
				if (n2.type == "string") {
					return Variable(Number(n1.string==n2.string));
				} else if (n2.type == "number") {
					return Variable(Number(std::to_string(n2.value)==n1.string));
				} else return Variable(Number(0));
			} else if (n1.type == "list") {
				if (n2.type == "list") {
					return Variable(Number(n1.list.values == n2.list.values));
				} else return Variable(Number(0));
			}
			return Variable(Number("Invalid operation '"+n1.type+"' = '" + n2.type + "'"));
		}
        case NodeType::n_NE: {
			Variable n1 = visit(node.nodes[0]);
			Variable n2 = visit(node.nodes[1]);
			if (n1.type == "number"){
				if (n2.type == "number") {
					return Variable(Number(n1.value!=n2.value));
				} else if (n2.type == "string") {
					return Variable(Number(std::to_string(n1.value)!=n2.string));
				} else return Variable(Number(0));
			} else if (n1.type == "string") {
				if (n2.type == "string") {
					return Variable(Number(n1.string!=n2.string));
				} else if (n2.type == "number") {
					return Variable(Number(std::to_string(n2.value)!=n1.string));
				} else return Variable(Number(0));
			} else if (n1.type == "list") {
				if (n2.type == "list") {
					return Variable(Number(n1.list.values != n2.list.values));
				} else return Variable(Number(0));
			}
			return Variable(Number("Invalid operation '"+n1.type+"' <> '" + n2.type + "'"));
		}
        case NodeType::n_LT:
            return Variable(Number(visit(node.nodes[0]).value < visit(node.nodes[1]).value));
        case NodeType::n_GT:
            return Variable(Number(visit(node.nodes[0]).value > visit(node.nodes[1]).value));
        case NodeType::n_LTE:
            return Variable(Number(visit(node.nodes[0]).value <= visit(node.nodes[1]).value));
        case NodeType::n_GTE:
            return Variable(Number(visit(node.nodes[0]).value >= visit(node.nodes[1]).value));
        case NodeType::n_AND:
            return Variable(Number(visit(node.nodes[0]).value && visit(node.nodes[1]).value));
        case NodeType::n_OR:
            return Variable(Number(visit(node.nodes[0]).value || visit(node.nodes[1]).value));
        case NodeType::n_NOT:
            return Variable(Number(!visit(node.nodes[0]).value));
        case NodeType::n_IF: {
            bool result = visit(node.nodes[0]).value; // get the condition
            if (result) return visit(node.nodes[1]);
            return Variable(Number(0));
        }
        case NodeType::n_IF_ELSE: {
            bool result = visit(node.nodes[0]).value;
            if (result) return visit(node.nodes[1]);
            return visit(node.nodes[2]);
        }
        case NodeType::n_FOR: {
            // assignment, end, commands, step
            visit(node.nodes[0]);
            while (globalSymbolTable.get(node.nodes[0].name).value != visit(node.nodes[1]).value) {
                visit(node.nodes[2]);
                globalSymbolTable.set(node.nodes[0].name,visit(node.nodes[3]));
                if (return_) break;
            }
            return return_value;
        }
        case NodeType::n_WHILE: {
            // comparison, commands
            while (visit(node.nodes[0]).value) {
                visit(node.nodes[1]);
                if (return_) break; // if inside func and return, break
            }
            return return_value;
        }
        case NodeType::n_PRINT: std::cout << visit(node.nodes[0]) << std::flush; return Variable(Number());
        case NodeType::n_READ: {
            std::string inp;
            std::getline(std::cin, inp);
            globalSymbolTable.set(node.name, Variable(inp));
            return Variable(Number());
        }
        case NodeType::n_INPUT: {
            float inp;
            std::string inp_s;
            std::getline(std::cin,inp_s);
            inp = std::stof(inp_s);
            globalSymbolTable.set(node.name, Variable(inp));
            return Variable(Number());
        }
        case NodeType::n_BLOCK: {
            for (long long i = 0; i < node.nodes.size(); i++) {
                if (node.nodes[i].type == NodeType::n_RETURN) return visit(node.nodes[i]);
                visit(node.nodes[i]);
            }
            return Variable(Number());
        }
        case NodeType::n_DEF: {
            Variable cnode = Variable(Function(node));
            globalSymbolTable.set(node.name, cnode);
            return cnode;
        }
        case NodeType::n_FUNCTION_CALL: {
			// first check whether it is special function system
			if (node.name == "shell") {
				Variable arg = visit(node.nodes[0].nodes[0]); // argument
				CommandResult cr = Command::exec("cd "+*cwd+';'+ arg.string.value);
				if (cr.exitstatus != 0) return Variable(Number(cr.exitstatus));
				return Variable(String(cr.output));
			} else if (node.name == "sizeof") {
				Variable arg = visit(node.nodes[0].nodes[0]);
				int val;
				if (arg.type == "string") {
					val = arg.string.value.size();
				} else if (arg.type == "number") {
					val = sizeof(arg.number.value);
				} else if (arg.type == "list") {
					val = arg.list.values.size();
				} else {
					return Variable(Number("Invalid type for special funciton sizeof: '"+arg.type+"'."));
				}
				return Variable(Number(val));
			}
            // get original function from global symboltable
            Function fun = globalSymbolTable.get(node.name).function;
            // get arg names
            std::vector<std::string> arg_names;
            for(Node n : fun.argus) {
                arg_names.push_back(n.name);
            }
            // copy global symbol table to fun smbt
            SymbolTable fun_smbt = globalSymbolTable;
            // evaluate all arg values
            // and plug said values into smbt, overriding global if needed
            Node args_ = node.nodes[0];
            for (int i = 0; i < args_.nodes.size(); i++) {
                fun_smbt.set(arg_names[i],visit(args_.nodes[i]));
            }
            // run function with its smbt
            // (i.e., new interpreter with
            // parent smbt as this smbt
            // and smbt normal as the function
            // object smbt)
            Interpreter tmp_fun = Interpreter(fun_smbt);
			tmp_fun.cwd = cwd;
            // call visit on the block of
            // function, which returns val
            tmp_fun.visit(fun.code.nodes[1]); // block - returns the value
            return tmp_fun.return_value;
        }
        case n_RETURN: {
            // global vars so can return.
            return_ = 1;
            return_value = visit(node.nodes[0]);
            return return_value;
        }
		case n_INCLUDE: {
			Variable lib = visit(node.nodes[0]);

			if (lib.type != "string") return Variable(Number("Type '"+lib.type+"' is not valid for library name."));
			std::string libname = lib.string.value;
			std::string filename;
			if (libname[0] == '_' && libname[libname.size()-1] == '_') filename = "stdlib/"+libname;
			else filename = *cwd+libname;
			std::string data = readFile(filename);
			if (data == "ERROR") {
				E_Module.throw_("Module '"+libname+"' not found.");
			}
			Lexer lexer = Lexer(data);
			std::vector<Token> tokens = lexer.generateTokens();
			if (tokens.size() == 0) return 0;
			/*std::cout << "[";
			for (int i = 0 ; i < tokens.size(); i++) {
				Token token = tokens[i];
				if (i == tokens.size()-1)
					std::cout << token;
				else
					std::cout << token << ", ";
			}
			std::cout << "]\n";*/
			Parser parser = Parser(tokens);
			Node tree = parser.parse();
			Interpreter interpreter = Interpreter(globalSymbolTable);
			interpreter.cwd = cwd;
        	Variable result = interpreter.visit(tree);
			SymbolTable inter = interpreter.globalSymbolTable;
			
			for(std::map<std::string,Variable>::const_iterator it = inter.m.begin(); it != inter.m.end(); ++it) {
				globalSymbolTable.set(it->first,it->second);
			}
			return Variable(Number());
			// TODO: ADD interpreter.globalSymbolTable to globalSymbolTable
			// TODO: check for redundant modules
		}
        default:
            return Variable(Number("Unknown runtime error."));

    };
}