#pragma once

#include "utils.h"

struct BinaryTreeNode {
  uint64_t hash;
  struct BinaryTreeNode *left;
  struct BinaryTreeNode *right;
  struct BinaryTreeNode *parent;
};

struct BinaryTree {
  struct BinaryTreeNode *root;
  size_t size;
};

bool BinaryTree_InternalFindPosition(struct BinaryTreeNode *node,  uint64_t hash, struct BinaryTreeNode **ret);
struct BinaryTreeNode* BinaryTree_Insert(struct BinaryTree *tree, uint64_t hash);
bool BinaryTree_InsertNode(struct BinaryTree *tree, struct BinaryTreeNode *node);
bool BinaryTree_Remove(struct BinaryTree *tree, uint64_t hash);
bool BinaryTree_RemoveNode(struct BinaryTree *tree, struct BinaryTreeNode *node);
bool BinaryTree_FindMax(struct BinaryTreeNode *node, struct BinaryTreeNode **ret);
bool BinaryTree_FindMin(struct BinaryTreeNode *node, struct BinaryTreeNode **ret);
void BinaryTree_SingleRotate_Left(struct BinaryTree *tree, struct BinaryTreeNode **top_node);
void BinaryTree_SingleRotate_Right(struct BinaryTree *tree, struct BinaryTreeNode **top_node);
void BinaryTree_DoubleRotate_Left(struct BinaryTree *tree, struct BinaryTreeNode **top_node);
void BinaryTree_DoubleRotate_Right(struct BinaryTree *tree, struct BinaryTreeNode **top_node);

void BinaryTree_Print_MidOrder(struct BinaryTreeNode *node);
bool BinaryTree_Check(struct BinaryTreeNode *node);