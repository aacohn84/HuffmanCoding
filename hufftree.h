#pragma once
#ifndef _HUFFTREE_H
#define _HUFFTREE_H

#include <map> // std::map -- used in generateHuffCodes
#include <utility> // std::pair -- used in generateHuffCodes
#include <fstream> // std::ofstream -- used in readFileBody
#include "bitops.h" // for reading/writing i/o streams in a bits-at-a-time fashion

template <class Key, class Value>
class HuffTree
{
private:
	class TreeNode // internal data structure, not referenced outside of HuffTree
	{
	public:
		Key key;
		Value value;
		TreeNode *left;
		TreeNode *right;
		
		TreeNode(Key k, Value v);
		TreeNode(Key k, Value v, TreeNode *left, TreeNode *right);
		TreeNode* copy();

	private:
		TreeNode* copy(TreeNode *src);
	};

private:
	TreeNode *root;

	void deleteTree(TreeNode *root);

public:
	HuffTree(Key root_key, Value root_value);
	~HuffTree();

	template <class Key, class Value>
	friend HuffTree<Key, Value>* join(const HuffTree<Key, Value> &ht1, const HuffTree<Key, Value> &ht2);

	Key rootkey() const;

	typedef std::pair<int, int> CodePair;
	typedef	std::map<Value, CodePair> CodeMap;

	CodeMap* generateHuffCodes();
	void writeFileHeader(obstream &outstream); // only works on the AsciiHuff variant of HuffTree
	static HuffTree* readFileHeader(ibstream &instream); // only works on the AsciiHuff variant of HuffTree
	void readFileBody(ibstream &instream, std::ofstream &outstream); // only works on the AsciiHuff variant of HuffTree 

private:
	HuffTree(); // this constructor only used by friends
	void generateHuffCodes(TreeNode *root, CodeMap &huffcodes, int length, int code);
	void writeFileHeader(TreeNode *root, obstream &outstream); // only works on the AsciiHuff variant of HuffTree
	static TreeNode* readFileHeaderHelper(ibstream &instream); // only works on the AsciiHuff variant of HuffTree
};

template <class Key, class Value>
HuffTree<Key, Value>::TreeNode::TreeNode(Key k, Value v)
	: key(k), value(v), left(NULL), right(NULL)
{
}

template <class Key, class Value>
HuffTree<Key, Value>::TreeNode::TreeNode(Key k, Value v, TreeNode *left, TreeNode *right)
	: key(k), value(v), left(left), right(right)
{
}

template <class Key, class Value>
typename HuffTree<Key, Value>::TreeNode* HuffTree<Key, Value>::TreeNode::copy()
{
	// return a deep copy of the current tree
	return copy(this);
}

template <class Key, class Value>
typename HuffTree<Key, Value>::TreeNode* HuffTree<Key, Value>::TreeNode::copy(TreeNode *src)
{
	if (src)
	{
		TreeNode *left = copy(src->left);
		TreeNode *right = copy(src->right);
		return new TreeNode(src->key, src->value, left, right);
	}
	return NULL;
}

template <class Key, class Value>
HuffTree<Key, Value>::HuffTree(Key root_key, Value root_value)
	: root(new TreeNode(root_key, root_value))
{
}

template <class Key, class Value>
HuffTree<Key, Value>::HuffTree()
{
	root = NULL;
}

template <class Key, class Value>
Key HuffTree<Key, Value>::rootkey() const
{
	return root->key;
}

template <class Key, class Value>
typename HuffTree<Key, Value>::CodeMap* HuffTree<Key, Value>::generateHuffCodes()
{
	CodeMap *huffcodes = NULL;
	if (root)
	{
		huffcodes = new CodeMap;
		generateHuffCodes(root, *huffcodes, 0, 0);
	}
	return huffcodes;
	/* TODO: instead of storing the codes as strings, store them as <length, code> pairs
		then when we do stream output, we can say writebits(length, code) */
}

template <class Key, class Value>
void HuffTree<Key, Value>::generateHuffCodes(TreeNode *root, CodeMap &huffcodes, int length, int code)
{
	if (root)
	{
		// do a depth-first search of the tree, building the code string along the way
		generateHuffCodes(root->left, huffcodes, length + 1, code << 1);
		generateHuffCodes(root->right, huffcodes, length + 1, (code << 1) | 1);

		if (root->left == NULL && root->right == NULL)
			huffcodes[root->value] = CodePair(length, code);
	}
}

template <class Key, class Value>
void HuffTree<Key, Value>::writeFileHeader(obstream &outstream)
{
	if (root)
		writeFileHeader(root, outstream);
}

// this method only works on the AsciiHuff variant of HuffTree
template <class Key, class Value>
void HuffTree<Key, Value>::writeFileHeader(TreeNode *root, obstream &outstream)
{
	if (root->left == NULL && root->right == NULL) // is it a leaf?
	{ // yes, write a 1 and the 9-bit Ascii+ value
		outstream.writebits(1, 1);
		outstream.writebits(9, int(root->value));
	}
	else
	{	// it's an internal node, write a zero
		outstream.writebits(1, 0);
			
		// internal nodes necessarily have 2 children -- write a header for each
		writeFileHeader(root->left, outstream);
		writeFileHeader(root->right, outstream);
	}
}

template <class Key, class Value>
HuffTree<Key, Value>* HuffTree<Key, Value>::readFileHeader(ibstream &instream)
{
	HuffTree *ht = new HuffTree();
	ht->root = readFileHeaderHelper(instream);
	return ht;
}

template <class Key, class Value>
typename HuffTree<Key, Value>::TreeNode* HuffTree<Key, Value>::readFileHeaderHelper(ibstream &instream)
{
	// read a 1 bit value
	int inbits;
	instream.readbits(1, inbits);
	if (inbits) // if a 1 is read, build a leaf node
	{
		instream.readbits(9, inbits);	// read a 9 bit value 
		return new TreeNode(0, inbits); // store it in a new node and return its address
	}
	else
	{ // create an internal node -- this node necessarily has 2 children
		TreeNode *left = readFileHeaderHelper(instream); 
		TreeNode *right = readFileHeaderHelper(instream);

		return new TreeNode(0, 0, left, right);
	}
}

template <class Key, class Value>
void HuffTree<Key, Value>::readFileBody(ibstream &instream, std::ofstream &outstream)
{
	using namespace std;

	bool hitEOF = false;
	while (!hitEOF)
	{
		int inbit;
		TreeNode *it = root;
		while (it->left != NULL && it->right != NULL)
		{
			instream.readbits(1, inbit);
			if (inbit == 0)
				it = it->left;
			else
				it = it->right;
		}
		if (it->value == 257) 
			hitEOF = true;
		else
			outstream.put(char(it->value));
	}
}

template <class Key, class Value>
HuffTree<Key, Value>::~HuffTree()
{
	deleteTree(root);
}

template <class Key, class Value>
void HuffTree<Key, Value>::deleteTree(TreeNode *root)
{
	if (root)
	{
		deleteTree(root->left);
		deleteTree(root->right);
		delete root;
	}
}

template <class Key, class Value>
HuffTree<Key, Value>* join(const HuffTree<Key, Value> &ht1, const HuffTree<Key, Value> &ht2)
{
	Key key;
	HuffTree<Key, Value> *ht_sum;
		
	key = ht1.root->key + ht2.root->key;
	ht_sum = new HuffTree<Key, Value>(key, Value(0));
	ht_sum->root->left = ht1.root->copy();
	ht_sum->root->right = ht2.root->copy();

	return ht_sum;
}

// #include "hufftree.cpp"

#endif