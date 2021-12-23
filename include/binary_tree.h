#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct BinaryTreeNode {
  uint64_t hash;
  struct BinaryTreeNode *left;
  struct BinaryTreeNode *right;
  struct BinaryTreeNode *parent;
};

struct BinaryTree {
  struct BinaryTreeNode *root;
  uint64_t size;
};

bool BinaryTree_InternalFindPosition(struct BinaryTreeNode *node,  uint64_t hash, struct BinaryTreeNode **ret);
bool BinaryTree_Insert(struct BinaryTree *tree, uint64_t hash);
bool BinaryTree_Remove(struct BinaryTree *tree, uint64_t hash);
bool BinaryTree_RemoveNode(struct BinaryTree *tree, struct BinaryTreeNode *node);
bool BinaryTree_FindMax(struct BinaryTreeNode *node, struct BinaryTreeNode **ret);
bool BinaryTree_FindMin(struct BinaryTreeNode *node, struct BinaryTreeNode **ret);