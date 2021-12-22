#include "Compiler.h"
#include "Machine.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[]) {
	//obrisi ovo
	string name = "321.432";
	double a = stoi(name);
	int i = 1;

	try {
		/*
		string infix = "2.5+6.7^x^y^q+8.9^9.1^d*s*k+z+5*6.93*x*7*g*t*i";
		vector<string> inf = *Compiler::infixVector(infix);
		Compiler::infix2postfix(inf);
		ExprTree* t = Compiler::constructTree(inf);
		ExprTree* tBal = Compiler::constructBalancedTree(inf);
		int a = 1, b = 1;
		Compiler c;
		ofstream* iFile = new ofstream("hprovera.txt", ios::out);
		c.writeTree(tBal, a, b, iFile);
		iFile->close();
		*/


		if (argc <= 2){
			cout << "The file names aren't entered." << endl;
			exit(1);
		}

		string txtFileName = argv[1];
		string imfFileName = txtFileName;
		imfFileName.replace(imfFileName.length() - 4, 4, ".imf");
		string configFileName = argv[2];
		int i = 0;
		
		/*
		//puca cim udje u execute fju, tacnije u load kad pokusa da otvori .imf fajl koji postoji sve vreme
		Machine::setConfigurations(configFileName);
		Compiler::compileAdvanced(txtFileName, imfFileName);
		//Compiler::compileSimple(txtFileName, imfFileName);
		while (1){
			Machine::execute(imfFileName);
			cout << i++;
			if (i == 253) system("pause");
		}
		*/

		/*
		//puca u compile fji, jer cita iz program fajla koji je odjednom prazan
		Machine::setConfigurations(configFileName);
		while (1){
			Compiler::compileAdvanced(txtFileName, imfFileName);
			//Compiler::compileSimple(txtFileName, imfFileName);
			Machine::execute(imfFileName);
			cout << i++;
			if (i == 169) system("pause");
		}
		*/
		
		/*
		//puca u set configurations, jer otvori prazan fajl
		while (1){
			Machine::setConfigurations(configFileName);
			Compiler::compileAdvanced(txtFileName, imfFileName);
			//Compiler::compileSimple(txtFileName, imfFileName);
			Machine::execute(imfFileName);
			cout<<i++;
			if (i == 127) system("pause");
		}
		*/
	}
	catch (exception& ex){
		cout << ex.what() << endl;
	}

	system("pause");
	return 0;
}

/*
int main(int argc, char* argv[]) {
//obrisi ovo
string name = "321.432";
double a = stoi(name);

try {
if (argc <= 2){
cout << "The file names aren't entered." << endl;
exit(1);
}
string txtFileName = argv[1];
string imfFileName = txtFileName;
imfFileName.replace(imfFileName.length() - 4, 4, ".imf");

cout << "1. Simple Compilation\n"
<< "2. Advanced Compilation\n"
<< "3. Exit\n"
<< "What is your choice?\n";

char choice; cin >> choice;

switch (choice){
case '1':
Compiler::compileAdvanced(txtFileName, imfFileName);
break;
case '2':
Compiler::compileSimple(txtFileName, imfFileName);
break;
default:
cout << "Incorrect input."
exit(4);
}

string configFileName = argv[2];
Machine::setConfigurations(configFileName);
Machine::execute(imfFileName);

}
catch (exception& ex){
cout << ex.what() << endl;
}
system("pause");
}
*/