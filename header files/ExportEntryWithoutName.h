#pragma once
#include <ostream>

#include "Composite.h"

class ExportEntryWithoutName : public Composite
{
private:
	int address, ordinal;
public:
	ExportEntryWithoutName(int address, int ordinal);
	~ExportEntryWithoutName();

	friend std::ostream& operator<<(std::ostream& out, ExportEntryWithoutName& exportEntry);

	virtual void Add(Composite* c) override;
	virtual void PrintInfoToStream(std::ostream& out) override;
};