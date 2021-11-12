#include <iostream>
#include <fstream>
#include <string>

#include "file.h"

std::string readFile(std::string file_name) {
	std::string text;
	std::string line;
	std::ifstream myfile(file_name);
	if (myfile.is_open()) {
		while ( std::getline (myfile,line) ) {
			text += line + '\n';
		}
		myfile.close();
	} else return "ERROR"; 
	return text;
}

std::string getWD(std::string file_name) {
	std::string med;
	std::string f;
	for (char c : file_name) {
		if (c == '/') {f += med + '/'; med = "";}
		else med += c;
	}
	return f;
}