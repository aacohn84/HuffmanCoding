/*
	Created on 3/3/2013

	Author: Aaron Cohn

	Summary: Implementation of class HuffTree, minus HuffTree::TreeNode. 
	TreeNode is implemented in HuffTreeNode.cpp.
*/

#include "hufftree.h"
#include "HuffPtrComparer.h"
#include <queue>
#include "bitops.h"
#include "globals.h"

using namespace std;

// Intermediate functions for building the Huffman Tree
Histogram* fileHistogram(ifstream &infile);
string code2str(const HuffTree::CodePair &cp);

HuffTree::HuffTree(int root_key, int root_value)
	: root(new TreeNode(root_key, root_value))
{
}

HuffTree::HuffTree()
{
	root = NULL;
}

int HuffTree::rootkey() const
{
	return root->key;
}

/* Compresses srcFile into destFile */
bool HuffTree::huff(const string &srcFileName, const string &destFileName)
{
	ifstream infile(srcFileName.c_str());
	obstream outfile(destFileName);

	if (!infile)
		return false;

	Histogram *hist = fileHistogram(infile);
	HuffPtr hufftree = buildHuffTree(*hist);

	hufftree->writeFileHeader(outfile);
	
	// reset the input stream
	infile.clear();
	infile.seekg(infile.beg);

	hufftree->compressFile(infile, outfile);

	infile.close();
	outfile.close();

	return true;
}

/* Uncompresses srcFile into destFile */
bool HuffTree::unhuff(const string &srcFileName, const string &destFileName)
{
	ibstream infile(srcFileName);
	ofstream outfile(destFileName);

	HuffPtr hufftree = HuffTree::treeFromHeader(infile);
	hufftree->decompressFile(infile, outfile);

	return true;
}

/* Generates huffman codes from tree */
HuffTree::CodeMap* HuffTree::generateHuffCodes() const
{
	CodeMap *huffcodes = NULL;
	if (root)
	{
		huffcodes = new CodeMap;
		generateHuffCodes(root, *huffcodes, 0, 0);
	}
	return huffcodes;
}

/* Traverses Huffman Tree recursively to generate minimal encodings */
void HuffTree::generateHuffCodes(TreeNode *root, CodeMap &huffcodes, int length, int code) const
{
	if (root)
	{
		// search for leaf, building code string along the way
		generateHuffCodes(root->left, huffcodes, length + 1, code << 1);
		generateHuffCodes(root->right, huffcodes, length + 1, (code << 1) | 1);

		// leaf found, huffman code complete
		if (root->left == NULL && root->right == NULL)
			huffcodes[root->value] = CodePair(length, code);
	}
}

/* Stores copy of tree in file header */
void HuffTree::writeFileHeader(obstream &outfile) const
{
	if (root)
		writeFileHeader(root, outfile);
}

/* Copies tree to file using recursive pre-order traversal */
void HuffTree::writeFileHeader(TreeNode *root, obstream &outfile) const
{
	if (root->left == NULL && root->right == NULL)
	{	// leaf, write 1 and 9-bit (Ascii+1) value
		outfile.writebits(1, 1);
		outfile.writebits(9, int(root->value));
	}
	else
	{	// internal node, write zero
		outfile.writebits(1, 0);
			
		// internal nodes necessarily have 2 children -- write header for each
		writeFileHeader(root->left, outfile);
		writeFileHeader(root->right, outfile);
	}
}

/* Compresses infile into outfile one char at a time using huffman codes */
void HuffTree::compressFile(ifstream &infile, obstream &outfile) const
{
	CodeMap *huffcodes = generateHuffCodes();

	if (infile)
	{
		char ch;
		CodePair codepair;
		int length, code;
		while (infile.get(ch))
		{	// write the huffcode for each character in the file
			codepair = (*huffcodes)[ch];
			length = codepair.first;
			code = codepair.second;
			outfile.writebits(length, code);
		}
		codepair = (*huffcodes)[PSEUDO_EOF];
		length = codepair.first;
		code = codepair.second;
		outfile.writebits(length, code);
	}
}

/* Creates huffman tree from header of huffed file */
HuffPtr HuffTree::treeFromHeader(ibstream &infile)
{
	HuffPtr ht = new HuffTree();
	ht->root = treeFromHeaderHelper(infile);
	return ht;
}

/* Recursively builds Huffman Tree from pre-order traversal in file header */
HuffTree::TreeNode* HuffTree::treeFromHeaderHelper(ibstream &infile)
{
	// read a 1 bit value
	int inbits;
	infile.readbits(1, inbits);
	if (inbits) // if a 1 is read, build a leaf node
	{
		infile.readbits(9, inbits);	// read a 9 bit value 
		return new TreeNode(0, inbits); // store it in a new node and return its address
	}
	else
	{ // create an internal node -- this node necessarily has 2 children
		TreeNode *left = treeFromHeaderHelper(infile); 
		TreeNode *right = treeFromHeaderHelper(infile);

		return new TreeNode(0, 0, left, right);
	}
}

/* Decompresses infile into outfile by traversing tree while reading codes */
void HuffTree::decompressFile(ibstream &infile, std::ofstream &outfile) const
{
	using namespace std;

	bool hitEOF = false;
	while (!hitEOF)
	{
		int inbit;
		TreeNode *it = root;
		while (it->left != NULL && it->right != NULL) // while child not reached
		{	// read bit, traverse tree
			infile.readbits(1, inbit);
			if (inbit == 0)
				it = it->left;
			else
				it = it->right;
		}
		if (it->value == PSEUDO_EOF) 
			hitEOF = true;
		else
			outfile.put(char(it->value));
	}
}

HuffTree::~HuffTree()
{
	deleteTree(root);
}

void HuffTree::deleteTree(TreeNode *root)
{
	if (root)
	{
		deleteTree(root->left);
		deleteTree(root->right);
		delete root;
	}
}

/*	Merges two trees by forming a new root node with ht1 as the left child and
	ht2 as the right child. */
HuffPtr HuffTree::join(const HuffTree &ht1, const HuffTree &ht2)
{
	int key;
	HuffPtr ht_sum;

	key = ht1.rootkey() + ht2.rootkey();
	ht_sum = new HuffTree(key, 0);

	// do the merge with copies of the trees
	ht_sum->root->left = ht1.root->copy();
	ht_sum->root->right = ht2.root->copy();

	return ht_sum;
}

/* Creates mapping of characters to frequencies (histogram) */
Histogram* fileHistogram(ifstream &infile)
{
	Histogram *hist = NULL;
	char ch;

	if (infile)
	{
		hist = new Histogram;
		while (infile.get(ch))
			(*hist)[ch] += 1;

		(*hist)[PSEUDO_EOF] = 1;
	}
	return hist;
}

HuffPtr HuffTree::buildHuffTree(const Histogram &hist)
{	
	typedef	priority_queue<HuffPtr, vector<HuffPtr>, HuffPtrComparer> HuffPtrPQ;

	HuffPtrPQ huff_pq;
	HuffPtr result = NULL;
	
	// create the initial "forest" of single-node trees
	auto begin = hist.begin(); auto end = hist.end();
	for (auto it = begin; it != end; it++)
	{
		int key = it->second;
		int val = it->first;

		// store a pointer to each tree in the priority queue
		huff_pq.push(new HuffTree(key, val));
	}
	
	// build a minimal encoding tree using Huffman's algorithm:
	// Repeat until 1 tree remains in the queue:
	//		pop off the two trees with the minimum keys
	//		merge them together, with the root being the sum of the keys
	//		put the new tree back into the queue.
	while (huff_pq.size() > 1)
	{
		auto u = huff_pq.top();
		huff_pq.pop();
		
		auto v = huff_pq.top();
		huff_pq.pop();
		
		auto z = HuffTree::join(*u, *v);
		huff_pq.push(z);
		
		delete u;
		delete v;
	}

	// the last item in the queue is the final huffman tree
	if (huff_pq.size() == 1)
		result = huff_pq.top();

	return result;
}

string code2str(const HuffTree::CodePair &cp)
{
	int length = cp.first;
	int code = cp.second;
	string codestr;
	
	for (int i = 0; i < length; i++)
	{
		if (code & 1) // is right-most bit a 1?
			codestr = "1" + codestr;
		else
			codestr = "0" + codestr;
		code = code >> 1;
	}

	return codestr;
}