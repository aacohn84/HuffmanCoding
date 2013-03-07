#pragma once
#ifndef _HUFFTREE_H
#define _HUFFTREE_H

/*
	Created on 3/3/2013

	Author:	Aaron Cohn

	Summary: HuffTree is a class which encapsulates the entire huffman coding
	compression scheme.
*/

#include <map>
#include <utility>	// std::pair -- used for internal types CodePair, CodeMap
#include <fstream>
#include "bitops.h"	// for reading/writing i/o streams in a bits-at-a-time fashion

class HuffTree
{
private:
// ---- Internal node representation ---- //
	class TreeNode
	{
	public:
		int key;
		int value;
		TreeNode *left;
		TreeNode *right;
		
		TreeNode(int k, int v);
		TreeNode(int k, int v, TreeNode *left, TreeNode *right);
		TreeNode* copy();

	private:
		TreeNode* copy(TreeNode *src);
	};

// ----- Huffman Tree ---- //
private:
	TreeNode *root;

public:
	// constructors
	HuffTree(int root_key, int root_value);
	~HuffTree();

	// accessors
	int rootkey() const;

	friend HuffTree* join(const HuffTree &ht1, const HuffTree &ht2);

	// convenient types
	typedef std::pair<int, int> CodePair;
	typedef	std::map<int, CodePair> CodeMap;

	// Public interface for compressing / decompressing files
	bool compress(std::string fileToCompress, std::string outputFileName);
	bool decompress(std::string fileToDecompress, std::string outputFileName);

	CodeMap* generateHuffCodes();
	void writeFileHeader(obstream &outstream);
	static HuffTree* readFileHeader(ibstream &instream);
	void readFileBody(ibstream &instream, std::ofstream &outstream);

private:
	// Internal methods -- not part of the public interface
	HuffTree();
	void generateHuffCodes(TreeNode *root, CodeMap &huffcodes, int length, int code);
	void writeFileHeader(TreeNode *root, obstream &outstream); 
	static TreeNode* readFileHeaderHelper(ibstream &instream);
	void deleteTree(TreeNode *root);
};

#endif