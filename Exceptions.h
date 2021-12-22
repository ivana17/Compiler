#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_
#include <exception>
#include <string>
using namespace std;
#include <iostream>

class VarNotAvailableException :public exception {
public:
	VarNotAvailableException(const char* msg) : exception(msg) {}
};

class MachineException : public exception{
public:
	MachineException(const char* msg) : exception(msg) {}
};

class FileNotFound : public exception{
public:
	FileNotFound(const char* msg) : exception(msg) {}
};


#endif _EXCEPTIONS_H_