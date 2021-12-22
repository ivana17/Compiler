#include "Compiler.h"
#include <stack>
#include <string>
#include <cstring>
#include <string.h>
#include <stack>
#include <vector>
#include <fstream>
#include <locale>
#include "Exceptions.h"
using namespace std;


// Cheks if it is an value assignment to a variable 
// or some expression
int Compiler::isValAssign(const string& str){
	int i = 0;
	while (str.at(i) != '\0'){
		if (!isdigit(str.at(i)))
			return 0;
		++i;
	}
	return 1;
}

int Compiler::priority(char c, bool isIpr){
	if (c == '+') return 2;
	if (c == '*') return 3;
	if (c == '^'){
		if (isIpr) return 5;
		else return 4;
	}
	return -1;
}

string* Compiler::blankRemove(const string& str){
	string* newStr = new string;
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] != ' ')
			*newStr += str[i];
		++i;
	}
	return newStr;
}

bool Compiler::isOperator(char c){
	if (c == '+' || c == '*' || c == '^')
		return true;
	return false;
}

bool Compiler::isNumber(const string& str){
	int i = 0;
	while ((str[i] != '\0') && (isdigit(str[i]) || str[i] == '.')) ++i;
	if (i != str.size()) return false;
	else return true;
}

// double
vector<string>* Compiler::infixVector(const string& infix){

	vector<string>* infVector=new vector<string>;
	int i = 0; 
	string hString = "";

	while (infix[i] != '\0'){
		if (isalnum(infix[i]) || infix[i] == '.'){
			hString = "";
			while (!isOperator(infix[i]) && infix[i] != '\0')
				hString += infix[i++];
			hString += '\0';
			infVector->push_back(hString);
		}
		else{
			hString = "";
			hString += infix[i++]; 
			hString += '\0';
			infVector->push_back(hString);
		}
	}
	infVector->push_back("\0");
	return infVector;
}

void Compiler::infix2postfix(vector<string>& infix){

	stack<string>* charStack = new stack<string>;
	int rank = 0;
	string x;
	unsigned infAt = 0;
	vector<string> postfix;

	string lastOp = ".";

	string next = infix[infAt++];
	while (next != "\0") {

		if (!isOperator(next[0])) {
			postfix.push_back(next);
			rank = rank + 1;
		}
		else{
			while (!charStack->empty() && (priority(next[0], true) <= priority(charStack->top()[0], false))) {
				if (next == "*" && lastOp == "*") { lastOp = "."; break; }
				x = charStack->top();
				charStack->pop();
				postfix.push_back(x);
				rank = rank - 1;
				if (rank < 1) {
					cout << "Nepravilan izraz";
				}
			}

			if (next != ")" || charStack->empty())
				charStack->push(next);
			else {
				x = charStack->top();
				charStack->pop();
			}
		}
		lastOp = next;
		next = infix[infAt++];
	}
	while (!charStack->empty()) {
		x = charStack->top();
		charStack->pop();
		postfix.push_back(x);
		rank = rank - 1;
	}
	if (rank != 1) {
		cout << "Nepravilan izraz";
	}

	postfix.push_back("\0");
	infix.clear();
	infix = postfix;

}

ExprTree* Compiler::constructTree(vector<string>& postfix){

	if (postfix.empty()) exit(8);
	stack<ExprTree*> nodeStack;
	ExprTree *node = nullptr, *child1 = nullptr, *child2 = nullptr;

	for (unsigned i = 0; i < postfix.size() - 1; i++) {

		// If operand, push into stack
		if (!isOperator(postfix.at(i)[0])) {
			node = new ExprTree(postfix[i]);
			nodeStack.push(node);
		}
		else {
			node = new ExprTree(postfix[i]);

			// Pop two top nodes
			child1 = nodeStack.top();
			nodeStack.pop();
			child2 = nodeStack.top();
			nodeStack.pop();

			//  make them children
			node->right = child1;
			node->left = child2;

			// Add this subexpression to stack
			nodeStack.push(node);
		}
	}
	// root
	node = nodeStack.top();
	nodeStack.pop();

	return node;
}

ExprTree* Compiler::constructBalancedTree(vector<string>& postfix){

	if (postfix.empty()) exit(8);
	stack<ExprTree*> nodeStack;
	ExprTree *node = nullptr, *child1 = nullptr, *child2 = nullptr;

	for (unsigned i = 0; i < postfix.size() - 1; i++) {

		// If operand, push into stack
		if (!isOperator(postfix.at(i)[0])) {
			node = new ExprTree(postfix[i]);
			nodeStack.push(node);
		}
		else {
			node = new ExprTree(postfix[i]);

			// Pop two top nodes
			child1 = nodeStack.top();
			nodeStack.pop();
			child2 = nodeStack.top();
			nodeStack.pop();

			//  make them children
			node->right = child1;
			node->left = child2;
			if (child1->h > child2->h)
				node->h = child1->h + 1;
			else
				node->h = child2->h + 1;
			node->balanceVal = child2->h - child1->h;
			
			//provera balansiranosti
			if (node->balanceVal > 1)
				node = rightRotation(node);

			if (node->balanceVal < -1)
				node = leftRotation(node);

			// Add this subexpression to stack
			nodeStack.push(node);
		}
	}
	// root
	node = nodeStack.top();
	nodeStack.pop();

	return node;
}

ExprTree* Compiler::rightRotation(ExprTree* node){
	if ((node->value[0] != node->left->value[0]) || (node->value[0] == '^') || (node->left->value[0] == '^')) return node;
	//prevezivanje
	ExprTree *oldRight = node->right, *oldRoot = node;
	ExprTree* ret = node->left;
	ExprTree* temp = ret->right;
	ret->right = node;
	node->left = temp;
	//menjanje visina
	if (oldRight->left && oldRight->right){
		if (oldRight->right->h > oldRight->left->h)
			oldRight->h = oldRight->right->h + 1;
		else
			oldRight->h = oldRight->left->h + 1;
		oldRight->balanceVal = oldRight->left->h - oldRight->right->h;
	}
	else oldRight->h = 0;
	if (oldRoot->right->h > oldRoot->left->h)
		oldRoot->h = oldRoot->right->h + 1;
	else
		oldRoot->h = oldRoot->left->h + 1;
	if (ret->right->h > ret->left->h)
		ret->h = ret->right->h + 1;
	else
		ret->h = ret->left->h + 1;
	if (temp->left && temp->right){
		if (temp->right->h > temp->left->h)
			temp->h = temp->right->h + 1;
		else
			temp->h = temp->left->h + 1;
	}
	//odredi balanse
	ret->balanceVal = ret->left->h - ret->right->h;
	oldRoot->balanceVal = oldRoot->left->h - oldRoot->right->h;

	//rotiraj po potrebi
	if (oldRoot->balanceVal > 1)
		ret->right = rightRotation(oldRoot);

	if (oldRoot->balanceVal < -1)
		ret->right = leftRotation(oldRoot);
	return ret;
}

ExprTree* Compiler::leftRotation(ExprTree* node){

	if ((node->value[0] != node->right->value[0]) || (node->value[0] == '^') || (node->right->value[0] == '^')) return node;
	//prevezivanje
	ExprTree *oldLeft = node->left, *oldRoot = node;
	ExprTree* ret = node->right;
	ExprTree* temp = ret->left;
	ret->left = node;
	node->right = temp;
	//menjanje visina
	if (oldLeft->left && oldLeft->right){
		if (oldLeft->right->h > oldLeft->left->h)
			oldLeft->h = oldLeft->right->h + 1;
		else
			oldLeft->h = oldLeft->left->h + 1;
		oldLeft->balanceVal = oldLeft->left->h - oldLeft->right->h;
	}
	else oldLeft->h = 0;
	if (oldRoot->right->h > oldRoot->left->h)
		oldRoot->h = oldRoot->right->h + 1;
	else
		oldRoot->h = oldRoot->left->h + 1;
	if (ret->right->h > ret->left->h)
		ret->h = ret->right->h + 1;
	else
		ret->h = ret->left->h + 1;
	if (temp->left && temp->right){
		if (temp->right->h > temp->left->h)
			temp->h = temp->right->h + 1;
		else
			temp->h = temp->left->h + 1;
	}

	ret->balanceVal = ret->left->h - ret->right->h;
	oldRoot->balanceVal = oldRoot->left->h - oldRoot->right->h;
	//rotiraj po potrebi
	if (oldRoot->balanceVal > 1)
		ret->left = rightRotation(oldRoot);

	if (oldRoot->balanceVal < -1)
		ret->left = leftRotation(oldRoot);
	return ret;
}
// Writes tree in .imf file
void Compiler::writeTree(ExprTree* node, int& iGlobal, int& tGlobal, ofstream* imfFile){

	int numOfIds = 0;
	struct idStruct{
		string id;
		idStruct* next, *prev;
		idStruct(){ id = ""; next = prev = nullptr; }
		~idStruct(){ next = prev = nullptr; }
	};

	idStruct* identificators = nullptr, *last = nullptr;


	if (node == nullptr)
		return;
	ExprTree* lastVisitedNode = nullptr;
	vector<ExprTree*>* unvisitedNodes = new vector<ExprTree*>;
	while (!unvisitedNodes->empty() || node != nullptr)
	{
		if (node != nullptr){
			unvisitedNodes->push_back(node);
			node = node->left;
		}
		else{
			ExprTree* topNode = unvisitedNodes->back();
			if (topNode->right != nullptr && lastVisitedNode != topNode->right) {
				node = topNode->right;
			}
			else {
				//adding new token
				idStruct* newId = new idStruct;
				++numOfIds; int i1 = 0;
				while (topNode->value[i1] != '\0')
					newId->id += topNode->value[i1++];

				if (!identificators) identificators = newId;
				else {
					last->next = newId;
					newId->prev = last;
				}
				last = newId;

				lastVisitedNode = topNode;
				unvisitedNodes->pop_back();

			}

			if (numOfIds >= 3) {

				bool s1 = isOperator(last->id[0]);
				bool s2 = !isOperator(last->prev->id[0]);
				bool s3 = !isOperator(last->prev->prev->id[0]);
				if (s1 && s2 && s3) {
					*imfFile << iGlobal++ << ": " << last->id[0] << " t" << tGlobal++ << ' ' << last->prev->prev->id << ' ' << last->prev->id << endl;
					//removing one token: operator, first operand and second operand
					for (int i = 0; i < 3; ++i) {
						--numOfIds;
						if (last == identificators) last = identificators = nullptr;
						else {
							idStruct* old = last; last = last->prev;
							last->next = nullptr;
							old = old->prev = nullptr;
						}
					}

					//adding new token id
					idStruct* newId = new idStruct;
					++numOfIds;
					newId->id += 't';
					newId->id += to_string(tGlobal - 1);
					if (!identificators) identificators = newId;
					else {
						last->next = newId;
						newId->prev = last;
					}
					last = newId;
				}
			}

		}
	}
	bool noUnvisitiedNodes = unvisitedNodes->empty();
	delete unvisitedNodes;
}

void Compiler::destroyTree(ExprTree* root){

	if (root == nullptr) return;
	stack<ExprTree*>* nodeStack = new stack<ExprTree*>;
	nodeStack->push(root);
	while (nodeStack->empty() == false) {
		ExprTree *node = nodeStack->top();
		nodeStack->pop();
		if (node->right)
			nodeStack->push(node->right);
		if (node->left)
			nodeStack->push(node->left);
		delete node;
	}

	root = nullptr;
	delete nodeStack;
}

//Compilation
void Compiler::compileSimple(const string& nameOfTxt, const string& nameOfImf){

	string hString;
	fstream program; ofstream* imfFile = nullptr;
	ifstream* inFile = new ifstream(nameOfTxt);

	if (inFile->good())
		program.open(nameOfTxt, ios::in);
	else throw FileNotFound("File not found.");

	Compiler* comp = new Compiler;

	imfFile = new ofstream(nameOfImf, ios::out);

	struct Ids{
		string id;
		string expr;
		Ids* next;
		Ids(){
			id = ""; expr = "";
			next = nullptr;
		}
		~Ids(){ next = nullptr; }
	};

	Ids* commands = nullptr, *last = nullptr;

	while (!program.eof()) {

		getline(program, hString);
		if (hString == "") throw FileNotFound("File is empty.");
		hString = *blankRemove(hString);

		int i = 0; string newId = "";
		while (hString[i] != '='){
			newId += hString[i++];
		}

		int exist = 0;
		Ids* hCom = commands;

		while (hCom != nullptr){
			if (hCom->id == newId) {
				exist = 1; break;
			}
			hCom = hCom->next;
		}
		//if this variable is already defined
		if (exist == 1){ continue; }

		//adding new command
		++i;
		Ids* newCom = new Ids;
		newCom->id = newId;
		while (hString[i] != '\0' && hString[i] != '\r')
			newCom->expr += hString[i], ++i;
		newCom->expr += '\0';

		if (!commands) commands = newCom;
		else last->next = newCom;
		last = newCom;
	}

	Ids *hCom = commands;
	int t = 1;
	int iter = 1;

	while (hCom){
		if (isValAssign(hCom->expr)){
			*imfFile << iter++ << ": = " << hCom->id << ' ' << hCom->expr << endl;
		}
		else{
			vector<string> expression = *infixVector(hCom->expr);
			infix2postfix(expression);
			ExprTree* ExprTreeRoot = constructTree(expression);
			comp->writeTree(ExprTreeRoot, iter, t, imfFile);
			*imfFile << iter++ << ": = " << hCom->id << " t" << to_string(t - 1) << endl;
		}
		hCom = hCom->next;
	}
	hCom = nullptr;

	//list deletion
	while (commands){
		Ids* old = commands;
		commands = commands->next;
		delete old;
	}

	program.close();
	imfFile->close();
	//delete imfFile;
}

void Compiler::compileAdvanced(const string& nameOfTxt, const string& nameOfImf){

	string hString;
	fstream program; ofstream* imfFile = nullptr;
	ifstream* inFile = new ifstream(nameOfTxt);

	if (inFile->good())
		program.open(nameOfTxt, ios::in);
	else throw FileNotFound("File not found.");

	Compiler* comp = new Compiler;

	imfFile = new ofstream(nameOfImf, ios::out);

	struct Ids{
		string id;
		int numOfDepend;
		string expr;
		Ids* next;
		Ids(){
			numOfDepend = 0; id = "";
			expr = ""; next = nullptr;
		}
		~Ids(){ next = nullptr; }
	};

	Ids* commands = nullptr, *last = nullptr;

	while (!program.eof()) {

		getline(program, hString);
		if (hString == "") throw FileNotFound("File is empty.");
		hString = *blankRemove(hString);

		int i = 0; string newId = "";
		while (hString[i] != '='){
			newId += hString[i++];
		}

		int exist = 0;
		Ids* hCom = commands;

		while (hCom != nullptr){
			if (hCom->id == newId) {
				exist = 1; break;
			}
			hCom = hCom->next;
		}
		// If this variable is already defined
		if (exist == 1){ continue; }

		// Adding new command
		++i;
		Ids* newCom = new Ids;
		newCom->id = newId;
		while (hString[i] != '\0' && hString[i] != '\r')
			newCom->expr += hString[i], ++i;
		newCom->expr += '\0';

		// Regulating dependency
		if (!isValAssign(newCom->expr)){
			int i = 0;
			vector<char> depVar;
			while (newCom->expr.at(i) != '\0'){
				if (isalpha(newCom->expr.at(i)))
				if (find(depVar.begin(), depVar.end(), newCom->expr.at(i)) == depVar.end())
					depVar.push_back(newCom->expr.at(i));
				++i;
			}

			while (!depVar.empty()){
				Ids* hCom = commands;
				while (hCom && (hCom->id[0] != depVar.back()))
					hCom = hCom->next;
				if (hCom)
					hCom->numOfDepend++;
				depVar.pop_back();
			}
		}

		if (!commands) commands = newCom;
		else last->next = newCom;
		last = newCom;
	}

	// Sort - making an optimal memory allocation schedule
	Ids* bound = commands;
	while (bound && isValAssign(bound->expr))
		bound = bound->next;
	for (Ids* i = commands; i && (i->next != bound); i = i->next)
	for (Ids* j = i->next; j && (j != bound); j = j->next)
	if (i->numOfDepend < j->numOfDepend){
		Ids* temp = new Ids;
		temp->expr = i->expr; temp->id = i->id; temp->numOfDepend = i->numOfDepend;
		i->expr = j->expr; i->id = j->id; i->numOfDepend = j->numOfDepend;
		j->expr = temp->expr; j->id = temp->id; j->numOfDepend = temp->numOfDepend;
	}

	Ids *hCom = commands;
	int t = 1;
	int iter = 1;

	while (hCom){
		if (isValAssign(hCom->expr)){
			*imfFile << iter++ << ": = " << hCom->id << ' ' << hCom->expr << endl;
		}
		else{

			vector<string> expression = *infixVector(hCom->expr);
			infix2postfix(expression);
			ExprTree* ExprTreeRoot = constructBalancedTree(expression);
			comp->writeTree(ExprTreeRoot, iter, t, imfFile);
			*imfFile << iter++ << ": = " << hCom->id << " t" << to_string(t - 1) << endl;
			destroyTree(ExprTreeRoot);
		}
		hCom = hCom->next;
	}
	hCom = nullptr;

	//list deletion
	while (commands){
		Ids* old = commands;
		commands = commands->next;
		delete old;
	}

	program.close();
	imfFile->close();
	//delete imfFile;
}



/*
string* Compiler::in2post(const string& infix){

stack<char>* charStack = new stack<char>;
int rank = 0, x;
int infAt = 0;
string postfix = "";

char lastOp = '.';

char next = infix[infAt++];
while (next != '\0') {

if (isalnum(next)) {
postfix += next;
rank = rank + 1;
}
else{
while (!charStack->empty() && (priority(next, true) <= priority(charStack->top(), false))) {
if (next == '*' && lastOp == '*') { lastOp = '.'; break; }
x = charStack->top();
charStack->pop();
postfix += x;
rank = rank - 1;
if (rank < 1) {
cout << "Nepravilan izraz";
}
}

if (charStack->empty() || next != ')')
charStack->push(next);
else {
x = charStack->top();
charStack->pop();
}
}
lastOp = next;
next = infix[infAt++];
}
while (!charStack->empty()) {
x = charStack->top();
charStack->pop();
postfix += x;
rank = rank - 1;
}
if (rank != 1) {
cout << "Nepravilan izraz";
}


string* newPointer = new string(postfix);
return newPointer;

}

// Construction of the tree
ExprTree* Compiler::constructTree(const string& postfix){

	if (postfix == "") exit(8);
	stack<ExprTree*> nodeStack;
	ExprTree *node = nullptr, *child1 = nullptr, *child2 = nullptr;

	for (unsigned i = 0; i < postfix.length(); i++) {

		// If operand, push into stack
		if (!isOperator(postfix[i])) {
			node = new ExprTree(postfix[i]);
			nodeStack.push(node);
		}
		else {
			node = new ExprTree(postfix[i]);

			// Pop two top nodes
			child1 = nodeStack.top();
			nodeStack.pop();
			child2 = nodeStack.top();
			nodeStack.pop();

			//  make them children
			node->right = child1;
			node->left = child2;

			// Add this subexpression to stack
			nodeStack.push(node);
		}
	}
	// root
	node = nodeStack.top();
	nodeStack.pop();

	return node;
}


ExprTree* Compiler::constructBalancedTree(const string& postfix){

	if (postfix == "") exit(8);
	stack<ExprTree*> nodeStack;
	ExprTree *node = nullptr, *child1 = nullptr, *child2 = nullptr;

	for (unsigned i = 0; i < postfix.length(); i++) {

		// If operand, push into stack
		if (!isOperator(postfix[i])) {
			node = new ExprTree(postfix[i]);
			nodeStack.push(node);
		}
		else {
			node = new ExprTree(postfix[i]);

			// Pop two top nodes
			child1 = nodeStack.top();
			nodeStack.pop();
			child2 = nodeStack.top();
			nodeStack.pop();

			//  make them children
			node->right = child1;
			node->left = child2;
			if (child1->h > child2->h)
				node->h = child1->h + 1;
			else
				node->h = child2->h + 1;
			node->balanceVal = child2->h - child1->h;
			// Add this subexpression to stack
			//provera balansiranosti
			if (node->balanceVal > 1)
				node = rightRotation(node);

			if (node->balanceVal < -1)
				node = leftRotation(node);
			nodeStack.push(node);
		}
	}
	// root
	node = nodeStack.top();
	nodeStack.pop();

	return node;
}

ExprTree* Compiler::rightRotation(ExprTree* node){
	if ((node->value != node->left->value) || (node->value == '^') || (node->left->value == '^'))return node;
	//prevezivanje
	ExprTree *oldRight = node->right, *oldRoot = node;
	ExprTree* ret = node->left;
	ExprTree* temp = ret->right;
	ret->right = node;
	node->left = temp;
	//menjanje visina
	if (oldRight->left && oldRight->right){
		if (oldRight->right->h > oldRight->left->h)
			oldRight->h = oldRight->right->h + 1;
		else
			oldRight->h = oldRight->left->h + 1;
		oldRight->balanceVal = oldRight->left->h - oldRight->right->h;
	}
	else oldRight->h = 0;
	if (oldRoot->right->h > oldRoot->left->h)
		oldRoot->h = oldRoot->right->h + 1;
	else
		oldRoot->h = oldRoot->left->h + 1;
	if (ret->right->h > ret->left->h)
		ret->h = ret->right->h + 1;
	else
		ret->h = ret->left->h + 1;
	if (temp->left && temp->right){
		if (temp->right->h > temp->left->h)
			temp->h = temp->right->h + 1;
		else
			temp->h = temp->left->h + 1;
	}
	//odredi balanse
	ret->balanceVal = ret->left->h - ret->right->h;
	oldRoot->balanceVal = oldRoot->left->h - oldRoot->right->h;

	//rotiraj po potrebi
	if (oldRoot->balanceVal > 1)
		ret->right = rightRotation(oldRoot);

	if (oldRoot->balanceVal < -1)
		ret->right = leftRotation(oldRoot);
	return ret;
}

ExprTree* Compiler::leftRotation(ExprTree* node){
	if ((node->value != node->right->value) || (node->value == '^') || (node->right->value == '^'))return node;
	//prevezivanje
	ExprTree *oldLeft = node->left, *oldRoot = node;
	ExprTree* ret = node->right;
	ExprTree* temp = ret->left;
	ret->left = node;
	node->right = temp;
	//menjanje visina
	if (oldLeft->left && oldLeft->right){
		if (oldLeft->right->h > oldLeft->left->h)
			oldLeft->h = oldLeft->right->h + 1;
		else
			oldLeft->h = oldLeft->left->h + 1;
		oldLeft->balanceVal = oldLeft->left->h - oldLeft->right->h;
	}
	else oldLeft->h = 0;
	if (oldRoot->right->h > oldRoot->left->h)
		oldRoot->h = oldRoot->right->h + 1;
	else
		oldRoot->h = oldRoot->left->h + 1;
	if (ret->right->h > ret->left->h)
		ret->h = ret->right->h + 1;
	else
		ret->h = ret->left->h + 1;
	if (temp->left && temp->right){
		if (temp->right->h > temp->left->h)
			temp->h = temp->right->h + 1;
		else
			temp->h = temp->left->h + 1;
	}

	ret->balanceVal = ret->left->h - ret->right->h;
	oldRoot->balanceVal = oldRoot->left->h - oldRoot->right->h;
	//rotiraj po potrebi
	if (oldRoot->balanceVal > 1)
		ret->left = rightRotation(oldRoot);

	if (oldRoot->balanceVal < -1)
		ret->left = leftRotation(oldRoot);
	return ret;
}
// Writes tree in .imf file
void Compiler::writeTree(ExprTree* node, int& iGlobal, int& tGlobal, ofstream* imfFile){
	int numOfIds = 0;
	struct idStruct{
		string id;
		idStruct* next, *prev;
		idStruct(){ id = ""; next = prev = nullptr; }
		~idStruct(){ next = prev = nullptr; }
	};

	idStruct* identificators = nullptr, *last = nullptr;


	if (node == nullptr)
		return;
	ExprTree* lastVisitedNode = nullptr;
	vector<ExprTree*>* unvisitedNodes = new vector<ExprTree*>;
	while (!unvisitedNodes->empty() || node != nullptr)
	{
		if (node != nullptr){
			unvisitedNodes->push_back(node);
			node = node->left;
		}
		else{
			ExprTree* topNode = unvisitedNodes->back();
			if (topNode->right != nullptr && lastVisitedNode != topNode->right) {
				node = topNode->right;
			}
			else {
				//adding new token
				idStruct* newId = new idStruct;
				++numOfIds;
				newId->id += topNode->value;
				if (!identificators) identificators = newId;
				else {
					last->next = newId;
					newId->prev = last;
				}
				last = newId;

				lastVisitedNode = topNode;
				unvisitedNodes->pop_back();
			}

			if (numOfIds >= 3) {

				bool s1 = isOperator(last->id[0]);
				bool s2 = !isOperator(last->prev->id[0]);
				bool s3 = !isOperator(last->prev->prev->id[0]);
				if (s1 && s2 && s3) {
					*imfFile << iGlobal++ << ": " << last->id << ' ' << 't' << tGlobal++ << ' ' << last->prev->prev->id << ' ' << last->prev->id << endl;
					//removing one token: operator, first operand and second operand
					for (int i = 0; i < 3; ++i) {
						--numOfIds;
						if (last == identificators) last = identificators = nullptr;
						else {
							idStruct* old = last; last = last->prev;
							old = old->prev = nullptr;
						}
					}

					//adding new token id
					idStruct* newId = new idStruct;
					++numOfIds;
					newId->id += 't';
					newId->id += to_string(tGlobal - 1);
					if (!identificators) identificators = newId;
					else {
						last->next = newId;
						newId->prev = last;
					}
					last = newId;
				}
			}

		}
	}
	bool noUnvisitiedNodes = unvisitedNodes->empty();
	delete unvisitedNodes;
}


//Compilation
void Compiler::compileSimple(const string& nameOfTxt, const string& nameOfImf){

	string hString;
	fstream program; ofstream* imfFile = nullptr;
	ifstream* inFile = new ifstream(nameOfTxt);

	if (inFile->good())
		program.open(nameOfTxt, ios::in);
	else throw FileNotFound("File not found.");

	Compiler* comp = new Compiler;

	imfFile = new ofstream(nameOfImf, ios::out);

	struct Ids{
		string id;
		string expr;
		Ids* next;
		Ids(){
			id = ""; expr = "";
			next = nullptr;
		}
		~Ids(){ next = nullptr; }
	};

	Ids* commands = nullptr, *last = nullptr;

	while (!program.eof()) {

		getline(program, hString);
		hString = *blankRemove(hString);

		int i = 0; string newId = "";
		while (hString[i] != '='){
			newId += hString[i++];
		}

		int exist = 0;
		Ids* hCom = commands;

		while (hCom != nullptr){
			if (hCom->id == newId) {
				exist = 1; break;
			}
			hCom = hCom->next;
		}
		if (exist == 1){ continue; }//if this variable is already defined

		//adding new command
		++i;
		Ids* newCom = new Ids;
		newCom->id = newId;
		while (hString[i] != '\0' && hString[i] != '\r')
			newCom->expr += hString[i], ++i;
		newCom->expr += '\0';

		if (!commands) commands = newCom;
		else last->next = newCom;
		last = newCom;
	}

	Ids *hCom = commands;
	int t = 1;
	int iter = 1;

	while (hCom){
		if (isValAssign(hCom->expr)){
			*imfFile << iter++ << ": = " << hCom->id << ' ' << hCom->expr << endl;
		}
		else{
			string postfix = *comp->in2post(hCom->expr);
			ExprTree* ExprTreeRoot = constructTree(postfix);
			comp->writeTree(ExprTreeRoot, iter, t, imfFile);
			*imfFile << iter++ << ": = " << hCom->id << " t" << to_string(t - 1) << endl;
		}
		hCom = hCom->next;
	}
	hCom = nullptr;

	//list deletion
	while (commands){
		Ids* old = commands;
		commands = commands->next;
		delete old;
	}

	program.close();
	imfFile->close();
	//delete imfFile;
}

void Compiler::compileAdvanced(const string& nameOfTxt, const string& nameOfImf){

	string hString;
	fstream program; ofstream* imfFile = nullptr;
	ifstream* inFile = new ifstream(nameOfTxt);

	if (inFile->good())
		program.open(nameOfTxt, ios::in);
	else throw FileNotFound("File not found.");

	Compiler* comp = new Compiler;

	imfFile = new ofstream(nameOfImf, ios::out);

	struct Ids{
		string id;
		int numOfDepend;
		string expr;
		Ids* next;
		Ids(){
			numOfDepend = 0; id = "";
			expr = ""; next = nullptr;
		}
		~Ids(){ next = nullptr; }
	};

	Ids* commands = nullptr, *last = nullptr;

	while (!program.eof()) {

		getline(program, hString);
		hString = *blankRemove(hString);

		int i = 0; string newId = "";
		while (hString[i] != '='){
			newId += hString[i++];
		}

		int exist = 0;
		Ids* hCom = commands;

		while (hCom != nullptr){
			if (hCom->id == newId) {
				exist = 1; break;
			}
			hCom = hCom->next;
		}
		// If this variable is already defined
		if (exist == 1){ continue; }

		// Adding new command
		++i;
		Ids* newCom = new Ids;
		newCom->id = newId;
		while (hString[i] != '\0' && hString[i] != '\r')
			newCom->expr += hString[i], ++i;
		newCom->expr += '\0';

		// Regulating dependency
		if (!isValAssign(newCom->expr)){
			int i = 0;
			vector<char> depVar;
			while (newCom->expr.at(i) != '\0'){
				if (isalpha(newCom->expr.at(i)))
				if (find(depVar.begin(), depVar.end(), newCom->expr.at(i)) == depVar.end())
					depVar.push_back(newCom->expr.at(i));
				++i;
			}

			while (!depVar.empty()){
				Ids* hCom = commands;
				while (hCom && (hCom->id[0] != depVar.back()))
					hCom = hCom->next;
				if (hCom)
					hCom->numOfDepend++;
				depVar.pop_back();
			}
		}

		if (!commands) commands = newCom;
		else last->next = newCom;
		last = newCom;
	}

	// Sort - making an optimal memory allocation schedule
	Ids* bound = commands;
	while (bound && isValAssign(bound->expr))
		bound = bound->next;
	for (Ids* i = commands; i && (i->next != bound); i = i->next)
	for (Ids* j = i->next; j && (j != bound); j = j->next)
	if (i->numOfDepend < j->numOfDepend){
		Ids* temp = new Ids;
		temp->expr = i->expr; temp->id = i->id; temp->numOfDepend = i->numOfDepend;
		i->expr = j->expr; i->id = j->id; i->numOfDepend = j->numOfDepend;
		j->expr = temp->expr; j->id = temp->id; j->numOfDepend = temp->numOfDepend;
	}

	Ids *hCom = commands;
	int t = 1;
	int iter = 1;

	while (hCom){
		if (isValAssign(hCom->expr)){
			*imfFile << iter++ << ": = " << hCom->id << ' ' << hCom->expr << endl;
		}
		else{
			string postfix = *comp->in2post(hCom->expr);
			ExprTree* ExprTreeRoot = constructBalancedTree(postfix);
			comp->writeTree(ExprTreeRoot, iter, t, imfFile);
			*imfFile << iter++ << ": = " << hCom->id << " t" << to_string(t - 1) << endl;
		}
		hCom = hCom->next;
	}
	hCom = nullptr;

	//list deletion
	while (commands){
		Ids* old = commands;
		commands = commands->next;
		delete old;
	}

	program.close();
	imfFile->close();
	//delete imfFile;
}

*/
