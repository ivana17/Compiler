#ifndef _COMPILER_H_
#define _COMPILER_H_
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

struct ExprTree{
	string value;
	ExprTree* left, *right;
	int balanceVal, h;
	ExprTree(string val) :
		value(val), left(nullptr), right(nullptr), balanceVal(0), h(0){}
};

class Compiler {
public:
	Compiler(){}
	static bool isOperator(char);
	// Cheks if it is an value assignment to a variable
	static int isValAssign(const string&);
	static string* blankRemove(const string&);
	static int priority(char, bool);
	static bool isNumber(const string&);
	string* in2post(const string&);
	static void infix2postfix(vector<string>&);
	static vector<string>* infixVector(const string&);
	// Construction of the tree
	static ExprTree* constructTree(vector<string>&);
	// Construction of the balanced tree
	static ExprTree* constructBalancedTree(vector<string>&);
	// Writes tree in .imf file
	void writeTree(ExprTree*, int&, int&, ofstream*);
	static void compileAdvanced(const string&, const string&);
	static void compileSimple(const string&, const string&);
	static void destroyTree(ExprTree*);
private:
	static ExprTree* leftRotation(ExprTree* node);
	static ExprTree* rightRotation(ExprTree* node);
};

#endif _COMPILER_H_

/*
// Construction of the tree
static ExprTree* constructTree(const string&);
// Construction of the balanced tree
static ExprTree* constructBalancedTree(const string&);
// Writes tree in .imf file
void writeTree(ExprTree*, int&, int&, ofstream*);
private:
static ExprTree* leftRotation(ExprTree* node);
static ExprTree* rightRotation(ExprTree* node);
*/