#include "../header files/ResourceEntry.h"

const char* GetResourceType(int id)
{
	switch (id)
	{
	case 1: return "CURSOR";
	case 2: return "BITMAP";
	case 3: return "ICON";
	case 4: return "MENU";
	case 5: return "DIALOG";
	case 6: return "STRING";
	case 7: return "FONTDIR";
	case 8: return "FONT";
	case 9: return "ACCELERATOR";
	case 10: return "RCDATA";
	case 11: return "MESSAGETABLE";
	case 12: return "GROUP CURSOR";
	case 14: return "GROUP ICON";
	case 16: return "VERSION";
	case 17: return "DLGINCLUDE";
	case 19: return "PLUGPLAY";
	case 20: return "VXD";
	case 21: return "ANICURSOR";
	case 22: return "ANIICON";
	case 23: return "HTML";
	case 24: return "MANIFEST";
	}
	return nullptr;
}

const char* GetResourceLanguage(int id)
{
	if (id == 0x409)
		return "English";
	return nullptr;
}

ResourceEntry::ResourceEntry(char* type, int idType, char* name, int idName, int size, int fileOffset, int idLanguage)
{
	if (!type)
		this->type = NULL, this->idType = idType;
	else
	{
		int ln = strlen(type);
		this->type = new char[ln + 1];
		strcpy(this->type, type);
		this->idType = -1;
	}

	if (!name)
		this->name = NULL, this->idName = idName;
	else
	{
		int ln = strlen(name);
		this->name = new char[ln + 1];
		strcpy(this->name, name);
		this->idName = -1;
	}

	this->size = size;
	this->fileOffset = fileOffset;
	this->idLanguage = idLanguage;
}

ResourceEntry::~ResourceEntry()
{
	if (type)
		delete[] type;
	if (name)
		delete[] name;
}

void ResourceEntry::Add(Composite* c)
{
	return;
}

void ResourceEntry::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, ResourceEntry& resource)
{
	if (resource.name)
		out << "Resource Name: " << resource.name;
	else
		out << "Resource ID: 0x" << std::hex << resource.idName;
	out << ", type: ";
	if (resource.type)
		out << resource.type;
	else
	{
		if (GetResourceType(resource.idType))
			out << std::hex << GetResourceType(resource.idType);
		else
			out << "Type:" << std::dec << resource.idType;
	}
	out << ", file offset: 0x" << std::hex << resource.fileOffset;
	out << ", size: 0x" << std::hex << resource.size;

	if (GetResourceLanguage(resource.idLanguage))
		out << ", language: " << GetResourceLanguage(resource.idLanguage) << std::dec;
	else
		out << ", language: 0x" << resource.idLanguage << std::dec;

	return out;
}
