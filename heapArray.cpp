#include "heapArray.h"

#define NULL 0

heapArray::heapArray()
{
	a_ = NULL;
	length_ = 0;
	heapsize = 0;
}

heapArray::heapArray(int n)
{
	heapsize = n;
	length_ = n;
	a_ = new heapElement[length_];
}

heapArray::heapArray(heapArray &copy)
{
	heapsize = copy.heapsize;
	length_ = copy.length_;

	a_ = new heapElement[length_];
	for (int i = 0; i < length_; i++)
		a_[i] = copy.a_[i];
}

heapArray::~heapArray(void)
{
	delete [] a_;
}

int& heapArray::operator[](int i)
{
	int j = i - 1;

	if (j >= 0 && j < length_)
		return a_[j].key;
	else
		throw "Index out of bounds in heapArray::operator[].";
}

int heapArray::length()
{
	return length_;
}