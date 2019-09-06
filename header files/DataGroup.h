#pragma once
#include <ostream>

#include "Composite.h"
#include "DataPair.h"
#include "List.h"

class DataGroup : public Composite
{
private:
	const char* groupInfo;
	List<Composite*>groupElements;
public:
	DataGroup(const char* groupInfo);
	~DataGroup();

	friend std::ostream& operator <<(std::ostream& out, DataGroup& g);

	virtual void Add(Composite* c);
	virtual void PrintInfoToStream(std::ostream& out);
};