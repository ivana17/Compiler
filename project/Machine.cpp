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
#include "Memory.h"
#include <iomanip>
using namespace std;

Time sumTime, mulTime, expTime;
Time memWriteTime, memReadTime;
int pReadN, pWriteN;

ofstream* Machine::logFile = nullptr;

// strToDouble returns duration of operation, for example: line = "Ts = 15", returns 15(int)
double Machine::strToDouble(string& line){
	unsigned i = 0;
	line = *Compiler::blankRemove(line);
	while ((i < line.size()) && (line[i] != '=')) ++i;
	if (line[i] != '=') exit(9);
	++i;
	string tLine = "";
	while (line[i] != '\0')
		tLine += line[i++];
	Time time = stod(tLine);
	return time;
}
// Sets durations of operations
void Machine::setConfigurations(const string& configFileName){

	fstream config; string hString;
	ifstream* inFile = new ifstream(configFileName);

	if (inFile->good())
		config.open(configFileName, ios::in);
	else throw FileNotFound("File not found.");

	getline(config, hString); 
	if (hString == "") throw FileNotFound("File is empty.");
	sumTime = strToDouble(hString);
	getline(config, hString); mulTime = strToDouble(hString);
	getline(config, hString); expTime = strToDouble(hString);
	getline(config, hString); memWriteTime = strToDouble(hString);
	getline(config, hString); memReadTime = strToDouble(hString);
	getline(config, hString); pReadN = (int)strToDouble(hString);
	getline(config, hString); pWriteN = (int)strToDouble(hString);

	config.close();
}

Token* Machine::newOperand(const string& tokenName, ID idOp) {
	Token* newToken = nullptr;
	if (Compiler::isNumber(tokenName))
		return new Token(tokenName, idOp, stoi(tokenName));
	else
		return new Token(tokenName, idOp);
}

void Machine::addNewOperation(const string& line) {

	// Unpack the line from imf file
	int i = 0; string tokenName = "", idOp = "";
	while (line[i] != ':') idOp += line[i++];
	i += 2; // to pass ':' and the blank after it
	char oper = line[i++]; ++i;
	if (oper != '='){ // to read the name of the token
		while (line[i] != ' ') tokenName += line[i++];
		++i; //to pass the blank 
	}
	else // if oper is '=' token won't have the name
		tokenName += '0';
	string firstOp = ""; while (line[i] != ' ') firstOp += line[i++]; ++i;
	string secOp = "";
	while ((line[i] != '\0') && (line[i] != '\r'))
		secOp += line[i++];
	ID opId = stoi(idOp);

	if (oper == '=') {
		ValueAssignment* newOp = new ValueAssignment(stoi(idOp), memWriteTime, newOperand(firstOp, opId), newOperand(secOp, opId));
		Token* newResult = newOperand(firstOp, stoi(idOp)); newOp->setResult(newResult);

		waitPool.push_back(newOp);
	}
	else if (oper == '^') {
		Exponentiation* newOp = new Exponentiation(stoi(idOp), expTime, newOperand(firstOp, opId), newOperand(secOp, opId));
		Token* newResult = newOperand(tokenName, stoi(idOp)); newOp->setResult(newResult);
		waitPool.push_back(newOp);
	}
	else if (oper == '+'){
		Summation* newOp = new Summation(stoi(idOp), sumTime, newOperand(firstOp, opId), newOperand(secOp, opId));
		Token* newResult = newOperand(tokenName, stoi(idOp)); newOp->setResult(newResult);
		waitPool.push_back(newOp);
	}
	else if (oper == '*') {
		Multiplication* newOp = new Multiplication(stoi(idOp), mulTime, newOperand(firstOp, opId), newOperand(secOp, opId));
		Token* newResult = newOperand(tokenName, stoi(idOp)); newOp->setResult(newResult);
		waitPool.push_back(newOp);
	}
	else
		throw MachineException("Non-existent operation.");

}
// Loads formation from imf file
void Machine::load(const string& imfFileName){

	fstream config; string newOperation;
	ifstream* inFile = new ifstream(imfFileName);

	if (inFile->good())
		config.open(imfFileName, ios::in);
	else throw FileNotFound("File not found.");

	while (!config.eof()){
		getline(config, newOperation);
		if (newOperation == "") break;
		addNewOperation(newOperation);
	}

	config.close(); inFile = nullptr;
}
// Unlocks the operations from waiting pool
void Machine::update(){

	for (unsigned iWait = 0; (iWait < waitPool.size()) && (completePool.size() != 0); ++iWait){
		if (completePool.back()->oper->getResToken()->getName() == waitPool.at(iWait)->getFirstOp()->getName())
			waitPool.at(iWait)->getFirstOp()->setValue(completePool.back()->oper->getResult());
		if (completePool.back()->oper->getResToken()->getName() == waitPool.at(iWait)->getSecOp()->getName())
			waitPool.at(iWait)->getSecOp()->setValue(completePool.back()->oper->getResult());
	}
}

bool Machine::opCompare(const completedOp* i, const completedOp* j){

	// If j starts before i
	if (i->startTime > j->startTime) return true;
	// Else if they start at the same time, but j finishes first 
	else if ((i->startTime == j->startTime) &&
		(i->startTime + i->oper->getTime() > j->startTime + j->oper->getTime()))
		return true;
	// Else if start and end time are the same, but index(j) < index(i)
	else if ((i->startTime == j->startTime) &&
		(i->startTime + i->oper->getTime() == j->startTime + j->oper->getTime())
		&& (i->oper->getID() > j->oper->getID()))
		return true;
	else return false;

}

void Machine::swap(vector<completedOp*>& vect, unsigned i, unsigned j){

	Operation* hOperation = vect.at(i)->oper;
	Time hTime = vect.at(i)->startTime;

	vect.at(i)->oper = vect.at(j)->oper;
	vect.at(i)->startTime = vect.at(j)->startTime;

	vect.at(j)->oper = hOperation;
	vect.at(j)->startTime = hTime;

	hOperation = nullptr;
}

void Machine::execute(const string& imfFileName){

	string logFileName = imfFileName;
	logFileName = logFileName.replace(logFileName.length() - 4, 5, ".log");

	Machine::logFile = new ofstream(logFileName, ios::out);
	Machine* machine = new Machine();
	machine->load(imfFileName);

	try {

		Time memoryWriteTime = 0;

		while (!machine->waitPool.empty()) {

			int parallelCount = 0, iter = 0;

			while (iter != machine->waitPool.size()){
				if (machine->waitPool.at(iter)->check()) {
					if (machine->waitPool.at(iter)->getSign() == '='){
						if (parallelCount == pWriteN){
							++iter; continue;
						}
						if ((Scheduler::Instance()->getCurTime() == memoryWriteTime) ||
							(Scheduler::Instance()->getCurTime() - memoryWriteTime >= memWriteTime)){
							++parallelCount;
							memoryWriteTime = Scheduler::Instance()->getCurTime();
						}
						else { ++iter; continue; }
					}
					Event::create(machine->waitPool.at(iter), machine->waitPool.at(iter)->getTime(), machine->waitPool.at(iter)->getID());
					auto* exeOp = machine->waitPool.at(iter);
					machine->waitPool.erase(machine->waitPool.begin() + iter);
					machine->execPool.push_back(exeOp);
				}
				else ++iter;
			}

			Scheduler::Instance()->processNow();

			// Checks which operations are done and transferring them to the complete pool
			int iter1 = 0;
			while (iter1 != machine->execPool.size()) {
				if (machine->execPool.at(iter1)->isDone()) {

					Time currtime = Scheduler::Instance()->getCurTime() - machine->execPool.at(iter1)->getTime();
					auto* exeOp = machine->execPool.at(iter1);
					completedOp* complOp = new completedOp(exeOp, currtime);
					machine->completePool.push_back(complOp);

					// Removes operation from execute pool
					machine->execPool.erase(machine->execPool.begin() + iter1);
					// Unlocks other operations, if both the operands are ready
					machine->update();
				}
				else ++iter1;
			}

		}
		
		// Sort of the completed pool
		for (unsigned i = 0; i < machine->completePool.size() - 1; ++i)
		for (unsigned j = i; j < machine->completePool.size(); ++j)
		if (machine->opCompare(machine->completePool.at(i), machine->completePool.at(j)))
			machine->swap(machine->completePool, i, j);
			

		// Prints in the log file
		for (unsigned it = 0; it != machine->completePool.size(); it++){
			//provera://obrisi posle
			cout << setw(5) << left << to_string(machine->completePool.at(it)->oper->getID()) + ":";
			cout << machine->completePool.at(it)->startTime;
			cout << '/' << machine->completePool.at(it)->startTime + machine->completePool.at(it)->oper->getTime() << endl;


			*Machine::logFile << setw(5) << left << to_string(machine->completePool.at(it)->oper->getID()) + ":";
			*Machine::logFile << machine->completePool.at(it)->startTime << '/';
			*Machine::logFile << machine->completePool.at(it)->startTime + machine->completePool.at(it)->oper->getTime() << endl;

		}

		for (unsigned it = 0; it != machine->completePool.size(); it++)
			delete machine->completePool[it];
		machine->completePool.clear();

		Memory::Instance()->release();
		delete machine;
	}
	catch (VarNotAvailableException& varExc){
		delete machine; Memory::Instance()->release();
		cout << "Variable not found, memory error.";  throw varExc;
	}
	catch (MachineException& machExc){
		delete machine; Memory::Instance()->release();
		cout << "Machine Exception";  throw machExc;
	}
}