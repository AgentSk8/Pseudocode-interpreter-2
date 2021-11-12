#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <map>

enum ErrorType {
	e_IllegalCharacter = 0,
	e_InvalidSyntax = 1,
	e_RuntimeError = 2,
	e_UndefinedError = 3,
	e_MathError = 4,
	e_ModuleNotFoundError = 5
};

extern std::map<ErrorType,std::string> ErrorTypeMap;

// OOP not needed, but done for scalability
class Error {
	public:
	ErrorType error_type;
	// TODO: where is the error?
	// have properties in Token and Node which carry its
	// position in a file so when we get to interpreter
	// it can return the filename, line and column in the error
	/*int line;
	int column;
	int file;
	std::string code;*/
	Error();
	Error(ErrorType et);
	void throw_(std::string message_);
};

#endif