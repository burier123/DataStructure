//
// Created by wangxz on 12/20/21.
//

#pragma once

#include "binary_tree.h"

struct AVLTree {
  struct AVLTreeNode *root;
  size_t size;
};

struct AVLTreeNode {
  struct BinaryTreeNode bt_node;
  int diff;
};

bool AVLTree_InsertNode(struct AVLTree *tree, struct AVLTreeNode *node);
bool AVLTree_Insert(struct AVLTree *tree, uint64_t hash);
bool AVLTree_RemoveNode(struct AVLTree *tree, struct AVLTreeNode *node);