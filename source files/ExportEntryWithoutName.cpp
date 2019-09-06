#include "../header files/ExportEntryWithoutName.h"

ExportEntryWithoutName::ExportEntryWithoutName(int address, int ordinal)
{
	this->address = address;
	this->ordinal = ordinal;
}

ExportEntryWithoutName::~ExportEntryWithoutName()
{
}

void ExportEntryWithoutName::Add(Composite* c)
{
	return;
}

void ExportEntryWithoutName::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, ExportEntryWithoutName& exportEntry)
{
	out << "_Ordinal_" << std::dec << exportEntry.ordinal << " Ord: " << exportEntry.ordinal << " RVA: 0x" << std::hex << exportEntry.address;
	return out;
}
