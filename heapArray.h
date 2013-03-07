#pragma once
#ifndef HEAP_ARRAY_INCLUDED_
#define HEAP_ARRAY_INCLUDED_

#ifndef NULL
#define NULL 0
#endif

/*
	An array with indexing from 1 through N, intended to be used as the 
	underlying data structure of a heap, hence the "heapsize" member, which
	represents the logical number of elements in the array, as opposed to its
	length, which is the total number of array slots allocated.

	The length of the array is increased by the grow() method when heapsize
	reaches 3/4ths of the size of the array.
*/
template <typename heapElement>
class heapArray
{
private:
	heapElement *a_;	// dynamically allocated array
	int length_;		// actual length of the array

public:
	int heapsize;		// logical # of elements in the heap
	
	heapArray()
	{
		a_ = NULL;
		length_ = 4;
		heapsize = 0;

		a_ = new heapElement[length_];
	}

	heapArray(int n)
	{
		heapsize = 0;
		length_ = n;
		a_ = new heapElement[length_];
	}

	heapArray(heapArray &copy)
	{
		heapsize = copy.heapsize;
		length_ = copy.length_;

		a_ = new heapElement[length_];
		for (int i = 0; i < length_; i++)
			a_[i] = copy.a_[i];
	}

	~heapArray(void)
	{
		delete [] a_;
	}

	void grow()
	{
		// allocate a bigger array
		length_ = length_ * length_ + 1;
		heapElement *newarray = new heapElement[length_];

		// copy elements into the new array
		for (int i = 0; i < length_; i++)
			newarray[i] = a_[i];

		// deallocate the old array and replace it with the new one
		delete [] a_;
		a_ = newarray;
	}

	heapElement& operator[](int i)
	{
		int j = i - 1;

		if (j >= 0 && j < length_)
			return a_[j];
		else
			throw "Index out of bounds in heapArray::operator[].";
	}

	void insert(int index, const heapElement item)
	{
		if (index >= length_)
			throw "Index out of bounds in heapArray::insert
	}

	int length() const
	{
		return length_;
	}
};

#endif