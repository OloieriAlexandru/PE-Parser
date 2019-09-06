#pragma once
#include <ostream>
#include <cstring>

#include "Composite.h"

class ExportEntryWithName : public Composite
{
private:
	char* name;
	int address, ordinal;
public:
	ExportEntryWithName(char* name, int address, int ordinal);
	~ExportEntryWithName();

	friend std::ostream& operator <<(std::ostream& out, ExportEntryWithName& exportEntry);

	virtual void Add(Composite* c) override;
	virtual void PrintInfoToStream(std::ostream& out) override;
};