#include "priorityqueue.h"

priorityqueue::priorityqueue(void)
{
}


priorityqueue::~priorityqueue(void)
{
}

/* Inserts a key,value pair into the priority queue */
void priorityqueue::insert(hist_elem item)
{
	if (heap.heapsize >= 3.0/4 * heap.length())
		heap.grow();
	
	// place the item at the bottom of the heap
	heap[++heap.heapsize] = item;

	// if heap property is violated, bubble up
	int i = heap.heapsize / 2;
	while (i >= 1 && heap[i].key < item.key)
	{
		swap(heap[i], item);
		i /= 2;
	}
}

/* removes the highest priority key,value pair, returns the value of the pair */
hist_elem priorityqueue::popMax()
{
	if (heap.heapsize < 1)
		throw "Error: heap underflow.";

	hist_elem max = heap[1];
	heap[1] = heap[heap.heapsize];
	heap.heapsize -= 1;
	maxHeapify(1);

	return max;
}

/* used to maintain the heap property */
void priorityqueue::maxHeapify(int i)
{
	int largest, l, r;
	bool exchange = false;

	do
	{
		if (exchange)
		{
			swap(heap[i], heap[largest]);
			i = largest;
		}

		l = left(i);
		r = right(i);

		if (l <= heap.heapsize && heap[l].key > heap[i].key)
			largest = l;
		else 
			largest = i;

		if (r <= heap.heapsize && heap[r].key > heap[largest].key)
			largest = r;
		
	} while (exchange = largest != i);
}

/* used to maintain the heap property */
void priorityqueue::minHeapify(int i)
{
	int smallest, l, r;
	bool exchange = false;

	do
	{
		if (exchange)
		{
			swap(heap[i], heap[smallest]);
			i = smallest;
		}

		l = left(i);
		r = right(i);

		if (l <= heap.heapsize && heap[l].key < heap[i].key)
			smallest = l;
		else 
			smallest = i;

		if (r <= heap.heapsize && heap[r].key < heap[smallest].key)
			smallest = r;
		
	} while (exchange = smallest != i);
}

/* returns the offset index of the left child of i */
int priorityqueue::left(int i)
{
	return 2*i;
}

/* returns the offset index of the right child of i */
int priorityqueue::right(int i)
{
	return 2*i + 1;
}

void priorityqueue::swap(hist_elem &a, hist_elem &b)
{
	hist_elem copy_a = a;
	a = b;
	b = copy_a;
}