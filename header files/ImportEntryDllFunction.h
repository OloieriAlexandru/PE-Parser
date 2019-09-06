#pragma once
#include <ostream>
#include <cstring>

#include "Composite.h"

class ImportEntryDllFunction : public Composite
{
private:
	char* name;
	int address;
public:
	ImportEntryDllFunction(char* name, int address);
	~ImportEntryDllFunction();

	friend std::ostream& operator<<(std::ostream& out, ImportEntryDllFunction& dllFunction);

	virtual void Add(Composite* c) override;
	virtual void PrintInfoToStream(std::ostream& out) override;
};