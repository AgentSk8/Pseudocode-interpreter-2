#include <iostream>

#include "error.h"

std::map<ErrorType,std::string> ErrorTypeMap = {
	{ e_IllegalCharacter, 	"Illegal Character Error" 	},
	{ e_InvalidSyntax, 		"Invalid Syntax Error" 		},
	{ e_RuntimeError, 		"Runtime Error" 			},
	{ e_MathError, 			"Math Error"	 			},
	{ e_UndefinedError, 	"Undefined Error" 			},
	{ e_ModuleNotFoundError, "Module Not Found Error" 	},
};


Error::Error() {
	error_type = e_UndefinedError;
}
Error::Error(ErrorType et) {
	error_type = et;
}
void Error::throw_(std::string message_) {
	std::cout << "\u001b[31m"; // red
	std::cout << ErrorTypeMap[error_type] << std::endl; // error type
	std::cout << "\t" << message_ << std::endl; // custom message
	std::cout << "\u001b[0m"; // reset

	std::exit(1); // bad as no gc, but I wasn't doing that anyways ¯\_(ツ)_/¯
}