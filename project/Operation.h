#ifndef _OPERATION_H_
#define _OPERATION_H_
#include <vector>
#include <string>
#include "Token.h"
#include "Types.h"
#include "Interfaces.h"
using namespace std;

class Operation : public ITimedElement {
	//ITimedElement counts the delay and notifies when the operation is performed
private:
	ID id;
	Time T;
protected:
	vector<Token*> operands;
	//A pointer to the Token whose value will be set as a result
	Token* result;
	//Returns the result of the operation
	virtual double ResOfOperation() = 0;
public:
	Operation(ID i, Time t, Token* firstOp, Token* secOp) :
		id(i), T(t), result(nullptr){
		operands.push_back(firstOp);
		operands.push_back(secOp);
	}
	ID getID() const { return id; }
	Time getTime() const { return T; }
	Token* getResToken(){ return result; }
	double getResult() const { return result->getValue(); }
	Token* getFirstOp(){ return operands[0]; }
	Token* getSecOp(){ return operands[1]; }
	void setResult(Token* res)  { result = res; }
	// Checks are operations are ready for executing
	virtual bool check() = 0;
	// Returns the sign of operation
	virtual char getSign() = 0;
	// Notifies when the operation is finished
	void notify(ID);
	// Cheks if the operation is done
	bool isDone();
	void deleteOp(){
		delete result; result = nullptr;
		unsigned i = 0;
		while (i < operands.size())
			delete operands[i];
		operands.clear();
	}
	~Operation(){
		deleteOp();
	}
};

class ValueAssignment : public Operation {
public:
	ValueAssignment(ID id, Time time, Token* firstOp, Token* secOp) :
		Operation(id, time, firstOp, secOp){};
	double ResOfOperation();
	char getSign(){ return '='; }
	bool check();
	~ValueAssignment(){}
};

class Summation : public Operation {
public:
	Summation(ID id, Time time, Token* firstOp, Token* secOp) :
		Operation(id, time, firstOp, secOp){};
	double ResOfOperation();
	char getSign(){ return '+'; }
	bool check();
	~Summation(){}
};

class Multiplication : public Operation {
public:
	Multiplication(ID id, Time time, Token* firstOp, Token* secOp) :
		Operation(id, time, firstOp, secOp){};
	double ResOfOperation();
	char getSign(){ return '*'; }
	bool check();
	~Multiplication(){}
};

class Exponentiation : public Operation {
public:
	Exponentiation(ID id, Time time, Token* firstOp, Token* secOp) :
		Operation(id, time, firstOp, secOp){};
	double ResOfOperation();
	char getSign(){ return '^'; }
	bool check();
	~Exponentiation(){}
};

#endif _OPERATION_H_
