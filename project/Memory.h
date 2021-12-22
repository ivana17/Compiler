#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "Operation.h"
#include <exception>
#include <map>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Memory {
	static Memory* instance;
	map<string, double> storage;

	Memory(const Memory&) = delete;
	Memory& operator=(const Memory&) = delete;
	Memory(Memory&& other) = delete;
	Memory& operator=(Memory&& other) = delete;
protected:
	Memory() {}
	~Memory() {
		storage.clear();
	}
public:
	static Memory* Instance();
	void set(string, double);
	double get(string);
	static void release();
};

#endif _MEMORY_H_