#pragma once
#include <ostream>
#include <cstring>

template <class T1, class T2>
class Pair
{
public:
	T1 first;
	T2 second;
	Pair()
	{
		memset(&first, 0, sizeof(first));
		memset(&second, 0, sizeof(second));
	}
	Pair(T1 f, T2 s)
	{
		first = f;
		second = s;
	}
	
	friend std::ostream& operator<<(std::ostream& out, Pair<T1, T2> pair)
	{
		out << pair.first << ' ' << pair.second;
		return out;
	}
};