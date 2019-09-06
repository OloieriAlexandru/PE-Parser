#pragma once
#include <ostream>

template <class T>
class List;

template <class T>
class ListIterator;

template <class T>
class ListNode
{
private:
	T info;
	ListNode* nextT;
	friend class List<T>;
	friend class ListIterator<T>;
public:
	ListNode()
	{
		nextT = NULL;
		memset(&info, 0, sizeof(info));
	}
	ListNode(T info)
	{
		this->info = info;
		nextT = NULL;
	}

	friend std::ostream& operator <<(std::ostream& out, const ListNode<T> node)
	{
		out << node.info;
		return out;
	}

	friend std::ostream& operator <<(std::ostream& out, const List<T>& l);
};

template <class T>
class ListIterator
{
public:
	ListNode<T>* currentNode;
	ListIterator(ListNode<T>*node)
	{
		currentNode = node;
	}
	ListIterator& operator++()
	{
		currentNode = currentNode->nextT;
		return *this;
	}
	bool operator != (ListIterator it)
	{
		return currentNode != it.currentNode;
	}
	T operator *()
	{
		return currentNode->info;
	}
};

template <class T>
class List
{
private:
	ListNode<T>* root;
	ListNode<T>* lastT;
	int numberOfElements;

	friend class ListIterator<T>;
public:
	List<T>()
	{
		root = NULL;
		lastT = NULL;
		numberOfElements = 0;
	}
	~List<T>()
	{
		ListNode<T>* p = root, *nxt;
		while (p)
		{
			nxt = p->nextT;
			delete p;
			p = nxt;
		}
	}

	void Add(T element)
	{
		++numberOfElements;
		if (!root)
			root = new ListNode<T>(element), lastT = root;
		else
			lastT->nextT = new ListNode<T>(element), lastT = lastT->nextT;
	}

	friend std::ostream& operator <<(std::ostream& out, const List<T>& l)
	{
		ListNode<T>* pointer = l.root;
		while (pointer)
		{
			out << *pointer << '\n';
			pointer = pointer->nextT;
		}
		return out;
	}

	int GetSize()
	{
		return numberOfElements;
	}

	ListIterator<T> begin()
	{
		ListIterator<T> it(root);
		return it;
	}

	ListIterator<T> end()
	{
		ListIterator<T> it(NULL);
		return it;
	}
};
