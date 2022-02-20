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

static inline void RBTree_MakeNodeRed(struct RBTree *tree, struct RBTreeNode *node) {
  if (node->color == RED) {
    return;
  }

  struct RBTreeNode *parent = node->bt_node.parent;
  if (parent == NULL) {
    node->color = RED;
    return;
  }

  struct RBTreeNode *brother = node->bt_node.parent->left == node ? node->bt_node.right : node->bt_node.left;
  struct RBTreeNode *bro_left = brother->bt_node.left;
  struct RBTreeNode *bro_right = brother->bt_node.right;
  if (bro_left != NULL && bro_left->color == RED) {
    struct RBTreeNode *top = parent;
    if (parent->bt_node.left == node) {
      BinaryTree_DoubleRotate_Left(tree, &top);
    } else {
      BinaryTree_SingleRotate_Right(tree, &top);
      brother->color = RED;
      bro_left->color = BLACK;
    }
    parent->color = BLACK;
    node->color = RED;
  } else if (bro_right != NULL && bro_right->color == RED) {
    struct RBTreeNode *top = parent;
    if (parent->bt_node.left == node) {
      BinaryTree_SingleRotate_Left(tree, &top);
      brother->color = RED;
      bro_right->color = BLACK;
    } else {
      BinaryTree_DoubleRotate_Right(tree, &top);
    }
    parent->color = BLACK;
    node->color = RED;
  } else {
    parent->color = BLACK;
    node->color = RED;
    brother->color = RED;
  }
}

static inline void RBTree_SwapNode(struct RBTreeNode *n1, struct RBTreeNode *n2) {
  struct RBTreeNode tmp1;
  struct RBTreeNode tmp2;
  memcpy(&tmp1, n1, sizeof(tmp1));
  memcpy(&tmp2, n2, sizeof(tmp2));
  memcpy(n1, &tmp2, sizeof(tmp2));
  memcpy(n2, &tmp1, sizeof(tmp1));
  n1->bt_node.hash = tmp1.bt_node.hash;
  n2->bt_node.hash = tmp2.bt_node.hash;
}

/*bool RBTree_RemoveNode(struct RBTree *tree, struct RBTreeNode *node) {
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
  } else {
    tree->root->color = RED;
  }

  struct RBTreeNode *parent = NULL, *child = tree->root, *brother, *del = NULL, *next;
  struct RBTreeNode tmp;
  uint64_t hash = node->bt_node.hash;
  //bool found = false;

  while (child != NULL) {
    if (del == NULL) {
      if (hash == child->bt_node.hash) {
        if (node != child) {
          return false;
        }
        del = child;
        if (child->bt_node.left == NULL) {
          if (child->bt_node.right == NULL) {
            break;
          } else {
            next = child->bt_node.right;
          }
        } else {
          next = child->bt_node.left;
        }
      } else {
        next = child->bt_node.hash < hash ? child->bt_node.left : child->bt_node.right;
      }
    } else {
      if (child->bt_node.hash < del->bt_node.hash) {
        if (child->bt_node.right == NULL) {

          memcpy(&tmp, child, sizeof(tmp));
          memcpy(child, del, sizeof(tmp));
          child->bt_node.hash = tmp.bt_node.hash;
          next = child->bt_node.left;
        } else {
          next = child->bt_node.right;
        }
      }
    }

    if ((child->bt_node.left != NULL && RBTREENODE_CAST(child->bt_node.left)->color == RED) ||
        (child->bt_node.right!= NULL && RBTREENODE_CAST(child->bt_node.right)->color == RED)) {

    } else {
      RBTree_MakeNodeRed(tree, child);
    }


    parent = child;
    child = next;
  }

  if (del == NULL) {
    return false;
  }

  RBTree_MakeNodeRed(tree, del);
  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, del);
  *p = NULL;

  done:
  tree->root->color = BLACK;
  --tree->size;
  return true;
}*/

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



