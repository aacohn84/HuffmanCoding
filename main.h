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
typedef		HuffTree<unsigned, unsigned>	AsciiHuff;
typedef		AsciiHuff*						AsciiHuffPtr;
typedef		map<unsigned, unsigned>			AsciiMap;

// Primary functions used for compressing/decompressing files
void huff(const string &infile);
void unhuff(const string &infile, const string &outfile);

// Intermediate functions for building the Huffman Tree
AsciiMap* fileHistogram(const string &filename);
AsciiHuffPtr asciiMap2Huff(const AsciiMap &amap);
string code2str(const AsciiHuff::CodePair &cp);

// Comparison class for putting AsciiHuffPtrs into a priority queue
class AsciiHuffPtrComparer
{
private:
	bool reverse;

public:
	AsciiHuffPtrComparer(const bool &rev = false) 
		: reverse(rev) 
	{
	}

	bool operator()(const AsciiHuffPtr &lhs, const AsciiHuffPtr &rhs)
	{
		// if reverse = false, we get a min priority queue, else it's a max priority queue
		return reverse ? 
			lhs->rootkey() < rhs->rootkey() : 
			lhs->rootkey() > rhs->rootkey();
	}
};

#endif