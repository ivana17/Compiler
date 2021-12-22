#ifndef _MACHINE_H_
#define _MACHINE_H_
#include "Operation.h"
#include <exception>
#include <map>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

struct completedOp{
	Operation* oper;
	Time startTime;
	completedOp(Operation* op, Time t)
		:startTime(t){
		oper = op;
		op = nullptr;
	}
	~completedOp(){
		delete oper;
		oper = nullptr;
	}
};

class Machine {
private:
	vector<Operation*> execPool;
	vector<Operation*> waitPool;
	vector<completedOp*> completePool;
	// Returns duration of operation
	static double strToDouble(string&);
	// Loads formation from imf file
	void load(const string&);
	void addNewOperation(const string&);
	Token* newOperand(const string&, ID);
	// Unlocks the operations from waiting pool
	void update();
	bool opCompare(const completedOp*, const completedOp*);
	void swap(vector<completedOp*>&, unsigned, unsigned);
public:
	// Sets durations of operations
	static void setConfigurations(const string&);
	static void execute(const string&);
	static ofstream* logFile;
};

#endif _MACHINE_H_