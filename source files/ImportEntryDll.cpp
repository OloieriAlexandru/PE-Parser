#include "../header files/ImportEntryDll.h"

ImportEntryDll::ImportEntryDll(char* name)
{
	int ln = strlen(name);
	this->name = new char[ln + 1];
	strcpy(this->name, name);
}

ImportEntryDll::~ImportEntryDll()
{
	delete[] name;
}

void ImportEntryDll::Add(Composite* c)
{
	dllFunctions.Add(c);
}

void ImportEntryDll::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, ImportEntryDll& dll)
{
	out << "DLL Name: " << dll.name << "\nDLL Functions: \n";
	auto it = dll.dllFunctions.begin();
	for (int i = 0; i < dll.dllFunctions.GetSize(); ++i)
	{
		(*it)->PrintInfoToStream(out);
		++it;
		out << '\n';
	}
	return out;
}
