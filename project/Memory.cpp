#include "Memory.h"
#include "Machine.h"
#include "Compiler.h"
#include "Operation.h"
#include "Token.h"
#include "Sched.h"
#include "Event.h"
#include <string>
#include <fstream>
#include <iostream>
#include "Exceptions.h"
using namespace std;

Memory* Memory::instance = nullptr;

Memory* Memory::Instance() {
	if (instance == nullptr)
		instance = new Memory();
	return instance;
}

void Memory::release(){
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void Memory::set(string varName, double val) {

	// If varName does not match the key of any element in the container, 
	// the function inserts a new element with that key
	storage[varName] = val;
}

double Memory::get(string varName) {

	for (auto iter = storage.begin(); iter != storage.end(); ++iter)
	if (iter->first == varName)
		return storage[varName];

	throw VarNotAvailableException("Variable not found.");
}

