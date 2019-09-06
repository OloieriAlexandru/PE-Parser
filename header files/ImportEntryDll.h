#pragma once
#include <ostream>
#include <cstring>

#include "Composite.h"
#include "List.h"

class ImportEntryDll : public Composite
{
private:
	char* name;
	List<Composite*>dllFunctions;
public:
	ImportEntryDll(char* name);
	~ImportEntryDll();

	friend std::ostream& operator<<(std::ostream& out, ImportEntryDll& dll);

	virtual void Add(Composite* c) override;
	virtual void PrintInfoToStream(std::ostream& out) override;
};