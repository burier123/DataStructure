//
// Created by wangxz on 12/31/21.
//

#include "rb_tree.h"

CREATE_MEMORY_POOL(RBTreeNode, 1024)

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
  ((struct RBTreeNode *)(grand_parent->bt_node.left))->color = RED;
  ((struct RBTreeNode *)(grand_parent->bt_node.right))->color = RED;

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
      ((struct RBTreeNode *)(child->bt_node.left))->color = BLACK;
      ((struct RBTreeNode *)(child->bt_node.right))->color = BLACK;

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

/*static inline void RBTree_InternalRemoveBalance(struct RBTreeNode *node, struct RBTreeNode *parent, struct RBTreeNode *brother) {

}*/

bool RBTree_RemoveNode(struct RBTree *tree, struct RBTreeNode *node) {
  struct RBTreeNode *del = node, *sub, *child = NULL, *brother = NULL;
  struct RBTreeNode *parent = node;
  bool iter_left = false;
  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, node);

  if (node->bt_node.right == NULL) {
    if (node->bt_node.left == NULL) {
      *p = NULL;
      --tree->size;
      return true;
    } else {
      child = (struct RBTreeNode *) node->bt_node.left;
      iter_left = true;
    }
  } else if (node->bt_node.left == NULL) {
    child = (struct RBTreeNode *) node->bt_node.right;
    iter_left = false;
  } else {
    child = (struct RBTreeNode *) node->bt_node.right;
    iter_left = false;
    brother = (struct RBTreeNode *) node->bt_node.left;
  }

  if (tree->root == parent && child->color == BLACK) {
    struct RBTreeNode *tmp;
    if (parent->bt_node.left == child) {
      BinaryTree_SingleRotate_Left(tree, &tmp);
    } else {
      BinaryTree_SingleRotate_Right(tree, &tmp);
    }
    tmp->color = BLACK;
    parent->color = RED;
  }

  while (!(del->bt_node.left == NULL && del->bt_node.right == NULL)) {
    if (child->color == BLACK) {
      struct RBTreeNode *left = child->bt_node.left;
      struct RBTreeNode *right = child->bt_node.right;
      struct RBTreeNode *bro_left = brother->bt_node.left;
      struct RBTreeNode *bro_right = brother->bt_node.right;
      if (left != NULL) {}

      if (left != NULL && left->color) {}
    }
  }

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