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

// forward declarations from bitops.h
class obstream;
class ibstream;

// forward delcaration for HuffPtr
class HuffTree;

// convenient types for external use
typedef		std::map<int, int>	Histogram;
typedef		HuffTree*			HuffPtr;

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
		
		// constructors
		TreeNode(int k, int v);
		TreeNode(int k, int v, TreeNode *left, TreeNode *right);

		// const methods
		TreeNode* copy() const;

	private:
		// static methods
		static TreeNode* copy(const TreeNode *src);
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

	// convenient types for internal use
	typedef		std::pair<int, int>			CodePair;
	typedef		std::map<int, CodePair>		CodeMap;

	// Public interface for compressing / decompressing files
	static bool huff(const std::string &srcFileName, const std::string &destFileName);
	static bool unhuff(const std::string &srcFileName, const std::string &destFileName);

private:
	// Internal methods -- not part of the public interface
	HuffTree();
	CodeMap* generateHuffCodes() const;
	void generateHuffCodes(TreeNode *root, CodeMap &huffcodes, int length, int code) const;
	void writeFileHeader(obstream &outstream) const;
	void writeFileHeader(TreeNode *root, obstream &outstream) const;
	void compressFile(std::ifstream &infile, obstream &outfile) const;
	void decompressFile(ibstream &instream, std::ofstream &outstream) const;
	void deleteTree(TreeNode *root);
	static HuffPtr join(const HuffTree &ht1, const HuffTree &ht2);
	static HuffPtr buildHuffTree(const Histogram &hist);
	static HuffPtr treeFromHeader(ibstream &instream);
	static TreeNode* treeFromHeaderHelper(ibstream &instream);
};

#endif