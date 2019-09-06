#include "../header files/ExportEntryWithName.h"

ExportEntryWithName::ExportEntryWithName(char* name, int address, int ordinal)
{
	int ln = strlen(name);
	this->name = new char[ln + 1];
	strcpy(this->name, name);

	this->address = address;
	this->ordinal = ordinal;
}

ExportEntryWithName::~ExportEntryWithName()
{
	delete[] name;
}

void ExportEntryWithName::Add(Composite* c)
{
	return;
}

void ExportEntryWithName::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, ExportEntryWithName& exportEntry)
{
	out << exportEntry.name << " Ord: " << exportEntry.ordinal << " RVA: 0x" << std::hex << exportEntry.address;
	return out;
}
