#include "Operation.h"
#include "Token.h"
#include "Machine.h"
#include "Memory.h"
#include "Event.h"
#include "Sched.h"
#include <cmath>
#include <iomanip>


// Checks do operands have a value == are operations are ready for executing
bool ValueAssignment::check() {
	//checks the second operand, first is variable
	if (operands[1]->getValue() != -1) return true;
	else return false;
}

bool Summation::check() {
	if ((operands[0]->getValue() != -1) &&
		(operands[1]->getValue() != -1)) return true;
	else return false;
}

bool Multiplication::check() {
	if ((operands[0]->getValue() != -1)
		&& (operands[1]->getValue() != -1)) return true;
	else return false;
}

bool Exponentiation::check() {
	if ((operands[0]->getValue() != -1)
		&& (operands[1]->getValue() != -1)) return true;
	else return false;
}

double ValueAssignment::ResOfOperation(){
	// writes in memory
	Memory::Instance()->set(operands[0]->getName(), operands[1]->getValue());
	return operands[1]->getValue(); // returns right operand value
}

double Summation::ResOfOperation() {
	double opResult = operands[0]->getValue() + operands[1]->getValue();
	return opResult;
}

double Multiplication::ResOfOperation() {
	double opResult = operands[0]->getValue() * operands[1]->getValue();
	return opResult;
}

double Exponentiation::ResOfOperation() {
	double opResult = pow(operands[0]->getValue(), operands[1]->getValue());
	return opResult;
}
// Cheks if the operation is done
bool Operation::isDone(){
	if (result && (result->getValue() != -1))
		return true;
	else return false;
}
// Notifies when the operation is finished
void Operation::notify(ID operationID) {

	// Sets the result when the operation is finished
	if (result)
		result->setValue(ResOfOperation());

	// Deletion of tokens after execution
	if (operands.size() == 1){
		delete operands[0];
		operands[0] = nullptr;
	}
	else if (operands.size() == 2){
		delete operands[0];
		operands[0] = nullptr;
		delete operands[1];
		operands[1] = nullptr;
	}

	operands.clear();

}
