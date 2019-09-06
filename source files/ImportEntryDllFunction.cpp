#include "../header files/ImportEntryDllFunction.h"

ImportEntryDllFunction::ImportEntryDllFunction(char* name, int address)
{
	int ln = strlen(name);
	this->name = new char[ln + 1];
	strcpy(this->name, name);
	
	this->address = address;
}

ImportEntryDllFunction::~ImportEntryDllFunction()
{
	delete[] name;
}

void ImportEntryDllFunction::Add(Composite* c)
{
	return;
}

void ImportEntryDllFunction::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, ImportEntryDllFunction& dllFunction)
{
	out << "     " << dllFunction.name << " RVA: 0x" << std::hex <<  dllFunction.address;
	return out;
}
