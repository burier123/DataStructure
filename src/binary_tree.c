#include "binary_tree.h"

bool BinaryTree_InternalFindPosition(struct BinaryTreeNode *node,  uint64_t hash, struct BinaryTreeNode **ret) {
  if (ret == NULL) {
    return false;
  }

  if (node == NULL) {
    *ret = NULL;
    return false;
  }

  if (hash == node->hash) {
    *ret = node;
    return true;
  } else if (hash > node->hash) {
    if (node->right == NULL) {
      *ret = node;
      return false;
    } else {
      return BinaryTree_InternalFindPosition(node->right, hash, ret);
    }
  } else {
    if (node->left == NULL) {
      *ret = node;
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

  struct BinaryTreeNode *pos;
  if (BinaryTree_InternalFindPosition(tree->root, hash, &pos)) {
    return false;
  }

  struct BinaryTreeNode *new_node = malloc(sizeof(struct BinaryTreeNode));
  if (!new_node) {
    return false;
  }
  memset(new_node, 0, sizeof(struct BinaryTreeNode));
  new_node->hash = hash;
  if (pos->hash > hash) {
    pos->left = new_node;
  } else {
    pos->right = new_node;
  }
  new_node->parent = pos;
  ++tree->size;
  return true;
}

bool BinaryTree_FindMax(struct BinaryTreeNode *node, struct BinaryTreeNode **ret) {
  if (ret == NULL) {
    return false;
  }

  if (node == NULL) {
    *ret = NULL;
    return false;
  }

  struct BinaryTreeNode *tmp = node;
  while (tmp->right != NULL) {
    tmp = tmp->right;
  }

  *ret = tmp;
  return true;
}

bool BinaryTree_FindMin(struct BinaryTreeNode *node, struct BinaryTreeNode **ret) {
  if (ret == NULL) {
    return false;
  }

  if (node == NULL) {
    *ret = NULL;
    return false;
  }

  struct BinaryTreeNode *tmp = node;
  while (tmp->left != NULL) {
    tmp = tmp->left;
  }

  *ret = tmp;
  return true;
}

bool BinaryTree_Remove(struct BinaryTree *tree, uint64_t hash) {
  if (tree == NULL || tree->size == 0) {
    return false;
  }

  struct BinaryTreeNode *target = NULL;
  if (!BinaryTree_InternalFindPosition(tree->root, hash, &target) || target == NULL) {
    return false;
  }

  struct BinaryTreeNode **p = NULL;
  if (target->parent != NULL) {
    if (target->parent->left == target) {
      p = &target->parent->left;
    } else {
      p = &target->parent->right;
    }
  } else {
    p = &tree->root;
  }

  if (target->left == NULL) {
    if (target->right == NULL) {
      *p = NULL;
    } else {
      *p = target->right;
      target->right->parent = NULL;
    }
  } else if (target->right == NULL) {
    *p = target->left;
    target->left->parent = NULL;
  } else {
    struct BinaryTreeNode *sub;
    if (rand() % 2) {
      if (BinaryTree_FindMax(target->left, &sub)) {
        if (target->left != sub) {
          sub->parent->right = sub->left;
          sub->parent = NULL;
          *p = sub;
          sub->left = target->left;
          sub->right = target->right;
        } else {
          sub->parent = NULL;
          *p = sub;
          sub->right = target->right;
        }
      } else {
        return false;
      }
    } else {
      if (BinaryTree_FindMin(target->right, &sub)) {
        if (target->right != sub) {
          sub->parent->left = sub->right;
          sub->parent = NULL;
          *p = sub;
          sub->left = target->left;
          sub->right = target->right;
        } else {
          sub->parent = NULL;
          *p = sub;
          sub->left = target->left;
        }
      } else {
        return false;
      }
    }
  }

  free(target);
  --tree->size;
  return true;
}

