//
// Created by wangxz on 12/31/21.
//

#pragma once

#include <assert.h>

#include "memory_pool.h"
#include "binary_tree.h"

enum RBTreeColor {
  RED = 1,
  BLACK
};

struct RBTreeNode {
  struct BinaryTreeNode bt_node;
  enum RBTreeColor color;
};

struct RBTree {
  struct RBTreeNode *root;
  size_t size;
};

bool RBTree_InsertNode(struct RBTree *tree, struct RBTreeNode *node);
struct RBTreeNode* RBTree_Insert(struct RBTree *tree, uint64_t hash);
//bool RBTree_RemoveNode(struct RBTree *tree, struct RBTreeNode *node);

bool RBTree_Check(struct RBTreeNode *node, int *n_black);
