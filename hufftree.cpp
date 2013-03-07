#include "hufftree.h"

using namespace std;

HuffTree::TreeNode::TreeNode(int k, int v)
	: key(k), value(v), left(NULL), right(NULL)
{
}

HuffTree::TreeNode::TreeNode(int k, int v, TreeNode *left, TreeNode *right)
	: key(k), value(v), left(left), right(right)
{
}

HuffTree::TreeNode* HuffTree::TreeNode::copy()
{
	// return a deep copy of the current tree
	return copy(this);
}

HuffTree::TreeNode* HuffTree::TreeNode::copy(TreeNode *src)
{
	if (src)
	{
		TreeNode *left = copy(src->left);
		TreeNode *right = copy(src->right);
		return new TreeNode(src->key, src->value, left, right);
	}
	return NULL;
}

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

bool HuffTree::compress(string fileToCompress, string outputFileName)
{
	ifstream infile(fileToCompress.c_str());
	obstream outfile;

	if (!infile)
		return false;

	return true;
}

bool decompress(string fileToDecompress, string outputFileName)
{
	return true;
}

HuffTree::CodeMap* HuffTree::generateHuffCodes()
{
	CodeMap *huffcodes = NULL;
	if (root)
	{
		huffcodes = new CodeMap;
		generateHuffCodes(root, *huffcodes, 0, 0);
	}
	return huffcodes;
}

void HuffTree::generateHuffCodes(TreeNode *root, CodeMap &huffcodes, int length, int code)
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


void HuffTree::writeFileHeader(obstream &outstream)
{
	if (root)
		writeFileHeader(root, outstream);
}

void HuffTree::writeFileHeader(TreeNode *root, obstream &outstream)
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

HuffTree* HuffTree::readFileHeader(ibstream &instream)
{
	HuffTree *ht = new HuffTree();
	ht->root = readFileHeaderHelper(instream);
	return ht;
}

HuffTree::TreeNode* HuffTree::readFileHeaderHelper(ibstream &instream)
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

void HuffTree::readFileBody(ibstream &instream, std::ofstream &outstream)
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

HuffTree* join(const HuffTree &ht1, const HuffTree &ht2)
{
	int key;
	HuffTree *ht_sum;
		
	key = ht1.root->key + ht2.root->key;
	ht_sum = new HuffTree(key, int(0));
	ht_sum->root->left = ht1.root->copy();
	ht_sum->root->right = ht2.root->copy();

	return ht_sum;
}