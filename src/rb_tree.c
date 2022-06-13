//
// Created by wangxz on 12/31/21.
//

#include "rb_tree.h"

CREATE_MEMORY_POOL(RBTreeNode, 1024)

#define RBTREENODE_CAST(node) ((struct RBTreeNode *)(node))

static inline void RBTree_InternalBalanceNode(struct RBTree *tree, struct RBTreeNode *parent, struct RBTreeNode *child) {
  struct RBTreeNode *grand_parent = (struct RBTreeNode *) parent->bt_node.parent;
  if (grand_parent->bt_node.left == parent) {
    if (parent->bt_node.left == child) {
      BinaryTree_SingleRotate_Right(tree, (struct BinaryTreeNode **) &grand_parent);
    } else {
      BinaryTree_DoubleRotate_Right(tree, (struct BinaryTreeNode **) &grand_parent);
    }
  } else {
    if (parent->bt_node.right == child) {
      BinaryTree_SingleRotate_Left(tree, (struct BinaryTreeNode **) &grand_parent);
    } else {
      BinaryTree_DoubleRotate_Left(tree, (struct BinaryTreeNode **) &grand_parent);
    }
  }
  grand_parent->color = BLACK;
  RBTREENODE_CAST(grand_parent->bt_node.left)->color = RED;
  RBTREENODE_CAST(grand_parent->bt_node.right)->color = RED;

}

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

  struct RBTreeNode *parent = NULL, *child = tree->root;

  while (child != NULL) {
    if (child->bt_node.left != NULL && ((struct RBTreeNode *)(child->bt_node.left))->color == RED &&
        child->bt_node.right != NULL && ((struct RBTreeNode *)(child->bt_node.right))->color == RED) {
      child->color = RED;
      RBTREENODE_CAST(child->bt_node.left)->color = BLACK;
      RBTREENODE_CAST(child->bt_node.right)->color = BLACK;

      if (parent == NULL && child->color == RED) {
        child->color = BLACK;
      } else if (parent->color == RED) {
        RBTree_InternalBalanceNode(tree, parent, child);
      }
    }

    parent = child;
    if (node->bt_node.hash > child->bt_node.hash) {
      child = (struct RBTreeNode *) child->bt_node.right;
    } else if (node->bt_node.hash < child->bt_node.hash) {
      child = (struct RBTreeNode *) child->bt_node.left;
    } else {
      return false;
    }
  }

  node->bt_node.parent = (struct BinaryTreeNode *) parent;
  node->bt_node.left = NULL;
  node->bt_node.right = NULL;
  node->color = RED;
  if (node->bt_node.hash > parent->bt_node.hash) {
    parent->bt_node.right = (struct BinaryTreeNode *) node;
  } else {
    parent->bt_node.left = (struct BinaryTreeNode *) node;
  }

  if (parent->color == RED) {
    RBTree_InternalBalanceNode(tree, parent, node);
  }

  ++tree->size;
  return true;
}

struct RBTreeNode* RBTree_Insert(struct RBTree *tree, uint64_t hash) {
  struct RBTreeNode *node = RBTreeNode_Malloc(&RBTreeNode_pool);
  memset(node, 0, sizeof(*node));
  node->bt_node.hash = hash;
  if (!RBTree_InsertNode(tree, node)) {
    RBTreeNode_Free(&RBTreeNode_pool, node);
    return NULL;
  }
  return node;
}

static inline void RBTree_SwapNode(struct RBTree *tree, struct RBTreeNode *n1, struct RBTreeNode *n2) {
  if (n1 == n2) return;

  if (n1->bt_node.parent == n2 || n2->bt_node.parent == n1) {
    if (n2->bt_node.parent == n1) {
      struct RBTreeNode *tmp = n1;
      n1 = n2;
      n2 = tmp;
    }

    *BinaryTree_GetParentUpdatePosition(tree, n2) = n1;
    n1->bt_node.parent = n2->bt_node.parent;
    if (n2->bt_node.left == n1) {
      if (n1->bt_node.left != NULL) {
        n1->bt_node.left->parent = n2;
      }

      if (n1->bt_node.right != NULL) {
        n1->bt_node.right->parent = n2;
      }

      if (n2->bt_node.right != NULL) {
        n2->bt_node.right->parent = n1;
      }

      n2->bt_node.left = n1->bt_node.left;
      n1->bt_node.left = n2;
      n1->bt_node.parent = n2->bt_node.parent;
      n2->bt_node.parent = n1;
      struct BinaryTreeNode *tmp = n1->bt_node.right;
      n1->bt_node.right = n2->bt_node.right;
      n2->bt_node.right = tmp;
    } else {
      if (n1->bt_node.left != NULL) {
        n1->bt_node.left->parent = n2;
      }

      if (n1->bt_node.right != NULL) {
        n1->bt_node.right->parent = n2;
      }

      if (n2->bt_node.left != NULL) {
        n2->bt_node.left->parent = n1;
      }

      n2->bt_node.right = n1->bt_node.right;
      n1->bt_node.right = n2;
      n1->bt_node.parent = n2->bt_node.parent;
      n2->bt_node.parent = n1;
      struct BinaryTreeNode *tmp = n1->bt_node.left;
      n1->bt_node.left = n2->bt_node.left;
      n2->bt_node.left = tmp;
    }

    enum RBTreeColor color = n1->color;
    n1->color = n2->color;
    n2->color = color;
    return;
  }

  *BinaryTree_GetParentUpdatePosition(tree, n2) = n1;
  *BinaryTree_GetParentUpdatePosition(tree, n1) = n2;
  struct RBTreeNode tmp1;
  struct RBTreeNode tmp2;
  memcpy(&tmp1, n1, sizeof(tmp1));
  memcpy(&tmp2, n2, sizeof(tmp2));
  memcpy(n1, &tmp2, sizeof(tmp2));
  memcpy(n2, &tmp1, sizeof(tmp1));
  n1->bt_node.hash = tmp1.bt_node.hash;
  n2->bt_node.hash = tmp2.bt_node.hash;

  if (n1->bt_node.left != NULL) {
    n1->bt_node.left->parent = n1;
  }

  if (n1->bt_node.right != NULL) {
    n1->bt_node.right->parent = n1;
  }

  if (n2->bt_node.left != NULL) {
    n2->bt_node.left->parent = n2;
  }

  if (n2->bt_node.right != NULL) {
    n2->bt_node.right->parent = n2;
  }
}

bool RBTree_RemoveNode(struct RBTree *tree, struct RBTreeNode *node) {
  if (tree->size == 0 || node == NULL) {
    return false;
  } else if (tree->size == 1) {
    if (node != tree->root) {
      return false;
    } else {
      tree->root = NULL;
      --tree->size;
      return true;
    }
  }

  struct RBTreeNode *sub = NULL, *iter_node = NULL;
  if (node->bt_node.left == NULL) {
    if (node->bt_node.right != NULL) {
      RBTree_SwapNode(tree, node, node->bt_node.right);
    }
  } else if (node->bt_node.right == NULL) {
    RBTree_SwapNode(tree, node, node->bt_node.left);
  } else {
    BinaryTree_FindMin(node->bt_node.right, &sub);
    RBTree_SwapNode(tree, node, sub);
    if (node->bt_node.right != NULL) {
      RBTree_SwapNode(tree, node, node->bt_node.right);
    }
  }
  iter_node = node;

  while (iter_node != NULL && iter_node->color == BLACK) {
    struct RBTreeNode *parent = iter_node->bt_node.parent, *brother;
    if (parent == NULL) break;

    if (parent->bt_node.left == iter_node) {
      brother = parent->bt_node.right;
      if (brother->color == RED) {
        BinaryTree_SingleRotate_Left(tree, &parent);
        parent->color = BLACK;
        parent = parent->bt_node.left;
        parent->color = RED;
        brother = parent->bt_node.right;
      }

      if (brother->bt_node.left != NULL && RBTREENODE_CAST(brother->bt_node.left)->color == RED) {
        struct RBTreeNode *top = parent;
        BinaryTree_DoubleRotate_Left(tree, &top);
        top->color = parent->color;
        parent->color = BLACK;
        break;
      } else if (brother->bt_node.right != NULL && RBTREENODE_CAST(brother->bt_node.right)->color == RED) {
        struct RBTreeNode *top = parent;
        BinaryTree_SingleRotate_Left(tree, &top);
        top->color = parent->color;
        parent->color = BLACK;
        RBTREENODE_CAST(brother->bt_node.right)->color = BLACK;
        break;
      } else {
        brother->color = RED;
        if (parent->color == RED) {
          parent->color = BLACK;
          break;
        }
      }
    } else {
      brother = parent->bt_node.left;
      if (brother->color == RED) {
        BinaryTree_SingleRotate_Right(tree, &parent);
        parent->color = BLACK;
        parent = parent->bt_node.right;
        parent->color = RED;
        brother = parent->bt_node.left;
      }

      if (brother->bt_node.right != NULL && RBTREENODE_CAST(brother->bt_node.right)->color == RED) {
        struct RBTreeNode *top = parent;
        BinaryTree_DoubleRotate_Right(tree, &top);
        top->color = parent->color;
        parent->color = BLACK;
        break;
      } else if (brother->bt_node.left != NULL && RBTREENODE_CAST(brother->bt_node.left)->color == RED) {
        struct RBTreeNode *top = parent;
        BinaryTree_SingleRotate_Right(tree, &top);
        top->color = parent->color;
        parent->color = BLACK;
        RBTREENODE_CAST(brother->bt_node.left)->color = BLACK;
        break;
      } else {
        brother->color = RED;
        if (parent->color == RED) {
          parent->color = BLACK;
          break;
        }
      }
    }

    iter_node = iter_node->bt_node.parent;
  }

  *BinaryTree_GetParentUpdatePosition(tree, node) = NULL;
  --tree->size;
  return true;
}

bool RBTree_Check(struct RBTreeNode *node, int *n_black) {
  if (node == NULL) {
    *n_black = 0;
    return true;
  }

  int nr, nl;
  if (!RBTree_Check((struct RBTreeNode *) node->bt_node.left, &nl)) {
    return false;
  }

  if (!RBTree_Check((struct RBTreeNode *) node->bt_node.right, &nr)) {
    return false;
  }

  if (nl != nr) {
    printf("RBTree check failed at %p, hash value:%lu\n",node, node->bt_node.hash);
    return false;
  }

  *n_black = node->color == RED ? nr : nr + 1;
  return true;
}



