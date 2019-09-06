#include "../header files/DataPair.h"

int DataPair::numberOfDigits(unsigned long long v)
{
	if (v == 0) return 1;
	int ans = 0;
	while (v)
		++ans, v /= 10;
	return ans;
}

DataPair::DataPair()
{
	key = NULL;
	value = NULL;
}

DataPair::DataPair(const char* k, const char* v)
{
	int ln = strlen(k);
	key = new char[ln + 1];
	strcpy(key, k);
	ln = strlen(v);
	value = new char[ln + 1];
	strcpy(value, v);
}

DataPair::DataPair(char* k, char* v)
{
	int ln = strlen(k);
	key = new char[ln + 1];
	strcpy(key, k);
	ln = strlen(v);
	value = new char[ln + 1];
	strcpy(value, v);
}

DataPair::DataPair(char* k, unsigned long long v)
{
	int ln = strlen(k);
	key = new char[ln + 1];
	strcpy(key, k);
	ln = numberOfDigits(v);
	value = new char[ln + 1];
	value[ln] = 0;
	for (int i = 0; i < ln; ++i)
		value[ln - i - 1] = (char)(v % 10 + '0');
}

DataPair::DataPair(const DataPair& p)
{
	int ln = strlen(p.key);
	key = new char[ln + 1];
	strcpy(key, p.key);
	ln = strlen(p.value);
	value = new char[ln + 1];
	strcpy(value, p.value);
}

DataPair::~DataPair()
{
	delete[] key;
	delete[] value;
}

DataPair& DataPair::operator=(const DataPair& p)
{
	int ln = strlen(p.key);
	key = new char[ln + 1];
	strcpy(key, p.key);
	ln = strlen(p.value);
	value = new char[ln + 1];
	strcpy(value, p.value);
	return *this;
}

void DataPair::Add(Composite* c)
{
	return;
}

void DataPair::PrintInfoToStream(std::ostream& out)
{
	out << *this;
}

std::ostream& operator<<(std::ostream& out, const DataPair& p)
{
	out << p.key << " : " << p.value;
	return out;
}
