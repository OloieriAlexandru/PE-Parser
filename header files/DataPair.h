#pragma once
#include <ostream>
#include <cstring>

#include "Composite.h"

class DataPair : public Composite
{
private:
	char* key;
	char* value;
	
	int numberOfDigits(unsigned long long v);
public:
	DataPair();
	DataPair(const char* k, const char* v);
	DataPair(char *k, char *v);
	DataPair(char* k, unsigned long long v);
	DataPair(const DataPair& p);
	~DataPair();

	DataPair& operator = (const DataPair& p);
	friend std::ostream& operator <<(std::ostream& out, const DataPair& p);

	virtual void Add(Composite* c) override;
	virtual void PrintInfoToStream(std::ostream& out) override;
};