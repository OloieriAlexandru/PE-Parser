#include "../header files/DataGroup.h"

DataGroup::DataGroup(const char* groupInfo)
{
	this->groupInfo = groupInfo;
}

DataGroup::~DataGroup()
{
	for (auto x : groupElements)
		delete x;
}

void DataGroup::Add(Composite* c)
{
	groupElements.Add(c);
}

void DataGroup::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, DataGroup& g)
{
	out << g.groupInfo << '\n';
	auto it = g.groupElements.begin();
	for (int i = 0; i < g.groupElements.GetSize(); ++i)
	{
		(*it)->PrintInfoToStream(out);
		++it;
		if (i != g.groupElements.GetSize() - 1)
			out << '\n';
	}
	return out;
}
