build:
	cd src;g++ -std=c++17 file.cpp error.cpp lexer.cpp parser.cpp interpreter.cpp main.cpp -o ../builds/main