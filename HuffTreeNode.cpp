#include "hufftree.h"

HuffTree::TreeNode::TreeNode(int k, int v)
	: key(k), value(v), left(NULL), right(NULL)
{
}

HuffTree::TreeNode::TreeNode(int k, int v, TreeNode *left, TreeNode *right)
	: key(k), value(v), left(left), right(right)
{
}

HuffTree::TreeNode* HuffTree::TreeNode::copy() const
{
	// return a deep copy of the current tree
	return copy(this);
}

HuffTree::TreeNode* HuffTree::TreeNode::copy(const TreeNode *src)
{
	if (src)
	{
		TreeNode *left = copy(src->left);
		TreeNode *right = copy(src->right);
		return new TreeNode(src->key, src->value, left, right);
	}
	return NULL;
}