#pragma once
#ifndef _MAIN_H
#define _MAIN_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <fstream>
#include "prompt.h"
#include "hufftree.h"

using namespace std;

// Convenience types
typedef		HuffTree*		HuffPtr;
typedef		map<int, int>	AsciiMap;

// Primary functions used for compressing/decompressing files
void huff(const string &infile);
void unhuff(const string &infile, const string &outfile);

// Intermediate functions for building the Huffman Tree
AsciiMap* fileHistogram(const string &filename);
HuffPtr asciiMap2Huff(const AsciiMap &amap);
string code2str(const HuffTree::CodePair &cp);

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

#endif