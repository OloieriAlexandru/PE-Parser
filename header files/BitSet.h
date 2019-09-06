#pragma once
#include <cstring>

class BitSet
{
private:
	unsigned * set;
	int size, maxBitIndex;

	int getRequiredSize(int x);
	int getIndexElement(int x);
public:
	BitSet(int bitCount);
	~BitSet();

	void Set(int bit);
	void Reset(int bit);
	bool Test(int bit);
};