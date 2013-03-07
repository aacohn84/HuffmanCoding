#pragma once

#include "heapArray.h"

class hist_elem
{
public:
	char sym;
	int key;
	bool operator<(hist_elem &right);
	bool operator>(hist_elem &right);
};

class priorityqueue
{
public:
	priorityqueue(void);
	~priorityqueue(void);
	void insert(hist_elem item);
	hist_elem popMax();
	hist_elem popMin();

private:
	heapArray<hist_elem> heap;

	void maxHeapify(int i);
	void minHeapify(int i);
	int left(int i);
	int right(int i);
	void swap(hist_elem &a, hist_elem &b);
};