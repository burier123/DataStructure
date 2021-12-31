#include "binary_tree.h"
#include "memory_pool.h"

CREATE_MEMORY_POOL(BinaryTreeNode, 1024)

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

struct BinaryTreeNode **BinaryTree_GetParentUpdatePosition(struct BinaryTree *tree, struct BinaryTreeNode *node) {
  if (node->parent != NULL) {
    if (node->parent->left == node) {
      return &node->parent->left;
    } else {
      return &node->parent->right;
    }
  } else {
    return &tree->root;
  }
}

struct BinaryTreeNode* BinaryTree_Insert(struct BinaryTree *tree, uint64_t hash) {
  if (tree == NULL) {
    return NULL;
  }

  if (!tree->size) {
    struct BinaryTreeNode *root = BinaryTreeNode_Malloc(&BinaryTreeNode_pool);
    if (!root) {
      return NULL;
    }
    memset(root, 0, sizeof(struct BinaryTreeNode));
    root->hash = hash;
    tree->root = root;
    tree->size = 1;
    return root;
  }

  struct BinaryTreeNode *pos;
  if (BinaryTree_InternalFindPosition(tree->root, hash, &pos)) {
    return NULL;
  }

  struct BinaryTreeNode *new_node = BinaryTreeNode_Malloc(&BinaryTreeNode_pool);
  if (!new_node) {
    return NULL;
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
  return new_node;
}

bool BinaryTree_InsertNode(struct BinaryTree *tree, struct BinaryTreeNode *node) {
  if (tree == NULL || node == NULL) {
    return false;
  }

  if (!tree->size) {
    tree->root = node;
    tree->size = 1;
    return true;
  }

  struct BinaryTreeNode *pos;
  if (BinaryTree_InternalFindPosition(tree->root, node->hash, &pos)) {
    return false;
  }

  if (pos->hash > node->hash) {
    pos->left = node;
  } else {
    pos->right = node;
  }
  node->parent = pos;
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

  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, target);

  if (target->left == NULL) {
    if (target->right == NULL) {
      *p = NULL;
    } else {
      *p = target->right;
      target->right->parent = target->parent;
    }
  } else if (target->right == NULL) {
    *p = target->left;
    target->left->parent = target->parent;
  } else {
    struct BinaryTreeNode *sub;
    if (rand() % 2) {
      if (BinaryTree_FindMax(target->left, &sub)) {
        if (target->left != sub) {
          sub->parent->right = sub->left;
          if (sub->left != NULL) {
            sub->left->parent = sub->parent;
          }
          sub->parent = target->parent;
          *p = sub;
          sub->left = target->left;
          sub->right = target->right;
          target->left->parent = sub;
          target->right->parent = sub;
        } else {
          sub->parent = target->parent;
          *p = sub;
          sub->right = target->right;
          target->right->parent = sub;
        }
      } else {
        return false;
      }
    } else {
      if (BinaryTree_FindMin(target->right, &sub)) {
        if (target->right != sub) {
          sub->parent->left = sub->right;
          if (sub->right != NULL) {
            sub->right->parent = sub->parent;
          }
          sub->parent = target->parent;
          *p = sub;
          sub->left = target->left;
          sub->right = target->right;
          target->left->parent = sub;
          target->right->parent = sub;
        } else {
          sub->parent = target->parent;
          *p = sub;
          sub->left = target->left;
          target->left->parent = sub;
        }
      } else {
        return false;
      }
    }
  }

  BinaryTreeNode_Free(&BinaryTreeNode_pool, target);
  --tree->size;
  return true;
}

bool BinaryTree_RemoveNode(struct BinaryTree *tree, struct BinaryTreeNode *node) {
  if (tree == NULL || tree->size == 0 || node == NULL) {
    return false;
  }

  struct BinaryTreeNode *target = NULL;
  if (!BinaryTree_InternalFindPosition(tree->root, node->hash, &target) || target != node) {
    return false;
  }

  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, target);

  if (target->left == NULL) {
    if (target->right == NULL) {
      *p = NULL;
    } else {
      *p = target->right;
      target->right->parent = target->parent;
    }
  } else if (target->right == NULL) {
    *p = target->left;
    target->left->parent = target->parent;
  } else {
    struct BinaryTreeNode *sub;
    if (rand() % 2) {
      if (BinaryTree_FindMax(target->left, &sub)) {
        if (target->left != sub) {
          sub->parent->right = sub->left;
          if (sub->left != NULL) {
            sub->left->parent = sub->parent;
          }
          sub->parent = target->parent;
          *p = sub;
          sub->left = target->left;
          sub->right = target->right;
          target->left->parent = sub;
          target->right->parent = sub;
        } else {
          sub->parent = target->parent;
          *p = sub;
          sub->right = target->right;
          target->right->parent = sub;
        }
      } else {
        return false;
      }
    } else {
      if (BinaryTree_FindMin(target->right, &sub)) {
        if (target->right != sub) {
          sub->parent->left = sub->right;
          if (sub->right != NULL) {
            sub->right->parent = sub->parent;
          }
          sub->parent = target->parent;
          *p = sub;
          sub->left = target->left;
          sub->right = target->right;
          target->left->parent = sub;
          target->right->parent = sub;
        } else {
          sub->parent = target->parent;
          *p = sub;
          sub->left = target->left;
          target->left->parent = sub;
        }
      } else {
        return false;
      }
    }
  }

  --tree->size;
  return true;
}

void BinaryTree_Print_MidOrder(struct BinaryTreeNode *node) {
  if (node == NULL) {
    return;
  }

  if (node->left != NULL) {
    BinaryTree_Print_MidOrder(node->left);
  }
  printf("%lu\n", node->hash);
  if (node->right != NULL) {
    BinaryTree_Print_MidOrder(node->right);
  }
}

bool BinaryTree_Check(struct BinaryTreeNode *node) {
  if (node == NULL) {
    return true;
  }

  if (node->left != NULL) {
    if (node->left->hash >= node->hash) {
      printf("left child check hash failed in %p", node);
      return false;
    }

    if (node->left->parent != node) {
      printf("left child check parent failed in %p", node);
      return false;
    }

    if (!BinaryTree_Check(node->left)) {
      return false;
    }
  }

  if (node->right != NULL) {
    if (node->right->hash <= node->hash) {
      printf("right child check hash failed in %p\n", node);
      return false;
    }

    if (node->right->parent != node) {
      printf("right child check parent failed in %p\n", node);
      return false;
    }

    if (!BinaryTree_Check(node->right)) {
      return false;
    }
  }

  return true;
}

void BinaryTree_SingleRotate_Left(struct BinaryTree *tree, struct BinaryTreeNode **top_node) {
  struct BinaryTreeNode *ori_top = *top_node;
  struct BinaryTreeNode *res_top = ori_top->right;
  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, ori_top);
  *p = res_top;
  ori_top->right = res_top->left;
  if (res_top->left != NULL) {
    res_top->left->parent = ori_top;
  }
  res_top->left = ori_top;
  res_top->parent = ori_top->parent;
  ori_top->parent = res_top;
  *top_node = res_top;
}

void BinaryTree_SingleRotate_Right(struct BinaryTree *tree, struct BinaryTreeNode **top_node) {
  struct BinaryTreeNode *ori_top = *top_node;
  struct BinaryTreeNode *res_top = ori_top->left;
  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, ori_top);
  *p = res_top;
  ori_top->left = res_top->right;
  if (res_top->right != NULL) {
    res_top->right->parent = ori_top;
  }
  res_top->right = ori_top;
  res_top->parent = ori_top->parent;
  ori_top->parent = res_top;
  *top_node = res_top;
}

void BinaryTree_DoubleRotate_Left(struct BinaryTree *tree, struct BinaryTreeNode **top_node) {
  struct BinaryTreeNode *res_left = *top_node;
  struct BinaryTreeNode *res_right = res_left->right;
  struct BinaryTreeNode *res_top = res_right->left;
  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, res_left);
  *p = res_top;
  res_left->right = res_top->left;
  if (res_top->left != NULL) {
    res_top->left->parent = res_left;
  }
  res_right->left = res_top->right;
  if (res_top->right != NULL) {
    res_top->right->parent = res_right;
  }
  res_top->left = res_left;
  res_top->right = res_right;
  res_top->parent = res_left->parent;
  res_left->parent = res_top;
  res_right->parent = res_top;
  *top_node = res_top;
}

void BinaryTree_DoubleRotate_Right(struct BinaryTree *tree, struct BinaryTreeNode **top_node) {
  struct BinaryTreeNode *res_right = *top_node;
  struct BinaryTreeNode *res_left = res_right->left;
  struct BinaryTreeNode *res_top = res_left->right;
  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, res_right);
  *p = res_top;
  res_left->right = res_top->left;
  if (res_top->left != NULL) {
    res_top->left->parent = res_left;
  }
  res_right->left = res_top->right;
  if (res_top->right != NULL) {
    res_top->right->parent = res_right;
  }
  res_top->left = res_left;
  res_top->right = res_right;
  res_top->parent = res_right->parent;
  res_left->parent = res_top;
  res_right->parent = res_top;
  *top_node = res_top;
}



