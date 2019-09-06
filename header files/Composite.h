#pragma once
#include <ostream>

class Composite
{
public:
	virtual ~Composite();

	virtual void Add(Composite* c) = 0;
	virtual void PrintInfoToStream(std::ostream& out) = 0;
};