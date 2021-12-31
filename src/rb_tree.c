//
// Created by wangxz on 12/31/21.
//

#include "rb_tree.h"

CREATE_MEMORY_POOL(RBTreeNode, 1024)

bool RBTree_InsertNode(struct RBTree *tree, struct RBTreeNode *node) {
  if (tree == NULL || node == NULL) {
    return false;
  }

  if (tree->size == 0) {
    tree->root = node;
    node->color = BLACK;
    tree->size = 1;
    return true;
  }

  struct RBTreeNode *parent = tree->root, *child;
  if (node->bt_node.hash > parent->bt_node.hash) {
    child = (struct RBTreeNode *) parent->bt_node.right;
  } else if (node->bt_node.hash < parent->bt_node.hash) {
    child = (struct RBTreeNode *) parent->bt_node.left;
  } else {
    return false;
  }

  while (child != NULL) {
    if (child->bt_node.left != NULL && ((struct RBTreeNode *)(child->bt_node.left))->color == RED &&
        child->bt_node.right != NULL && ((struct RBTreeNode *)(child->bt_node.right))->color == RED) {
      child->color = RED;
      ((struct RBTreeNode *)(child->bt_node.left))->color = BLACK;
      ((struct RBTreeNode *)(child->bt_node.right))->color = BLACK;

      if (parent->color == RED) {

      }
    }

    if (node->bt_node.hash > child->bt_node.hash) {

    }
  }

  return true;
}

struct RBTreeNode* RBTree_Insert(struct RBTree *tree, uint64_t hash) {

}

bool RBTree_RemoveNode(struct RBTree *tree, struct RBTreeNode *node) {

}

bool RBTree_Check(struct RBTreeNode *node, int *n_black) {

}