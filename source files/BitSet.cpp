#include "../header files/BitSet.h"

int BitSet::getRequiredSize(int x)
{
	int r = x % 32;
	if (!r) return x / 32;
	return (x + (32 - r)) / 32;
}

int BitSet::getIndexElement(int x)
{
	++x;
	return x / 32 + (x % 32 ? 1 : 0) - 1;
}

BitSet::BitSet(int bitCount)
{
	size = getRequiredSize(bitCount);
	set = new unsigned[size];
	memset(set, 0, sizeof(set));
	maxBitIndex = 32 * size;
}

BitSet::~BitSet()
{
	delete[] set;
}

void BitSet::Set(int bit)
{
	int index = getIndexElement(bit);
	set[index] |= (1u<<(bit % 32));
}

void BitSet::Reset(int bit)
{
	int index = getIndexElement(bit);
	set[index] &= (4294967295 ^ (1u << (bit % 32)));
}

bool BitSet::Test(int bit)
{
	int index = getIndexElement(bit);
	return (set[index] & (1u << (bit % 32)));
}
