#include "hufftree.h"

// Comparison class for putting HuffPtrs into a priority queue
class HuffPtrComparer
{
private:
	bool reverse;

public:
	HuffPtrComparer(const bool &rev = false) 
		: reverse(rev) 
	{
	}

	bool operator()(const HuffPtr &lhs, const HuffPtr &rhs)
	{
		// if reverse = false, we get a min priority queue, else it's a max priority queue
		return reverse ? 
			lhs->rootkey() < rhs->rootkey() : 
			lhs->rootkey() > rhs->rootkey();
	}
};