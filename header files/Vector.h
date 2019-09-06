#pragma once

template <class T>
class VectorIterator
{
public:
	T* elements;
	int currentElement, totalElements;
	VectorIterator(T* elements, int currentElement, int totalElements) 
		:elements(elements), currentElement(currentElement), totalElements(totalElements)
	{
	}
	VectorIterator& operator++()
	{
		++currentElement;
		return *this;
	}
	bool operator != (VectorIterator it)
	{
		return it.elements != elements || (it.elements == elements && it.currentElement != currentElement);
	}
	T operator *()
	{
		return elements[currentElement];
	}
};

template <class T>
class Vector
{
protected:
	T* elements;
	int allocatedSize, currentSize;

	void resize(int numberOfElementsToBeAdded)
	{
		int totalElements = currentSize + numberOfElementsToBeAdded;
		int oldAllocatedSize = allocatedSize;
		if (!allocatedSize)
			allocatedSize = 1;
		while (allocatedSize < totalElements)
			allocatedSize *= 2;
		if (allocatedSize != oldAllocatedSize)
		{
			T* newMemoryAddress = new T[allocatedSize];
			for (int i = 0; i < currentSize; ++i)
				newMemoryAddress[i] = elements[i];
			if (elements)
				delete[] elements;
			elements = newMemoryAddress;
		}
	}
public:
	Vector<T>()
	{
		elements = NULL;
		allocatedSize = currentSize = 0;
	}
	~Vector<T>()
	{
		delete[] elements;
	}

	void Add(T element)
	{
		resize(1);
		elements[currentSize++] = element;
	}

	T& operator [](int index)
	{
		if (index < 0 || index >= currentSize)
			return nullptr;
		return T[index];
	}

	void GetVectorAsCharArray(char** charArray)
	{
		return;
	}
	void RemoveCharArray(char** charArray)
	{
		return;
	}

	VectorIterator<T> begin()
	{
		VectorIterator<T> it(elements, 0, currentSize);
		return it;
	}
	VectorIterator<T> end()
	{
		VectorIterator<T> it(elements, currentSize, currentSize);
		return it;
	}
};

template<>
void Vector<char>::GetVectorAsCharArray(char** charArray)
{
	(*charArray) = new char[currentSize + 1];
	(*charArray)[currentSize] = '\0';
	for (int i = 0; i < currentSize; ++i)
		(*charArray)[i] = elements[i];
}

template<>
void Vector<char>::RemoveCharArray(char** charArray)
{
	if (*charArray)
	{
		delete[] * charArray;
		*charArray = NULL;
	}
}