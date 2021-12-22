#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include "Types.h"
#include "Interfaces.h"
#include "Operation.h"
using namespace std;

class Operation;

class Token {
public:
	Token(string name, ID opId, double val = -1) :
		name(name), opId(opId), value(val) {}
	double getValue() const{ return value; }
	string getName() const{ return name; }
	void setValue(double val){ value = val; }
	~Token(){}
private:
	ID opId;
	double value;
	string name;
};

#endif _TOKEN_H_

