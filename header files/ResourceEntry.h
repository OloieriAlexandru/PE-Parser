#pragma once
#include <ostream>
#include <cstring>

#include "Composite.h"

class ResourceEntry : public Composite
{
private:
	char* type, *name;
	int idType, idName, idLanguage, size, fileOffset;
public:
	ResourceEntry(char *type, int idType, char *name, int idName, int size, int fileOffset, int idLanguage);
	~ResourceEntry();

	friend std::ostream& operator <<(std::ostream& out, ResourceEntry& resource);

	virtual void Add(Composite* c) override;
	virtual void PrintInfoToStream(std::ostream& out) override;
};