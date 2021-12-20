#include "binary_tree.h"

bool BinaryTree_InternalFindPosition(struct BinaryTreeNode *node,  uint64_t hash, struct BinaryTreeNode ***ret) {
  if (ret == NULL) {
    return false;
  }

  if (node == NULL) {
    *ret = NULL;
    return false;
  }

  if (hash == node->hash) {
    *ret = &node;
    return true;
  } else if (hash > node->hash) {
    if (node->right == NULL) {
      *ret = &node->right;
      return false;
    } else {
      return BinaryTree_InternalFindPosition(node->right, hash, ret);
    }
  } else {
    if (node->left == NULL) {
      *ret = &node->left;
      return false;
    } else {
      return BinaryTree_InternalFindPosition(node->left, hash, ret);
    }
  }
}

bool BinaryTree_Insert(struct BinaryTree *tree, uint64_t hash) {
  if (!tree->size) {
    struct BinaryTreeNode *root = malloc(sizeof(struct BinaryTreeNode));
    if (!root) {
      return false;
    }
    memset(root, 0, sizeof(struct BinaryTreeNode));
    root->hash = hash;
    tree->root = root;
    tree->size = 1;
    return true;
  }

  struct BinaryTreeNode **pos;
  if (BinaryTree_InternalFindPosition(tree->root, hash, &pos)) {
    return false;
  }

  struct BinaryTreeNode *new_node = malloc(sizeof(struct BinaryTreeNode));
  if (!new_node) {
    return false;
  }
  memset(new_node, 0, sizeof(struct BinaryTreeNode));
  new_node->hash = hash;
  *pos = new_node;
  return true;
}

