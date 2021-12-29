//
// Created by wangxz on 12/20/21.
//

#include "avl_tree.h"
#include "memory_pool.h"

CREATE_MEMORY_POOL(AVLTreeNode, 1024)

bool AVLTree_InsertNode(struct AVLTree *tree, struct AVLTreeNode *node) {
  if (tree == NULL || node == NULL) {
    return false;
  }

  if (tree->size == 0) {
    tree->root = node;
    tree->size = 1;
    return true;
  }

  struct AVLTreeNode *pos;
  if (BinaryTree_InternalFindPosition((struct BinaryTreeNode *) tree->root, node->bt_node.hash, (struct BinaryTreeNode **) &pos)) {
    return false;
  }


  if (pos->bt_node.hash > node->bt_node.hash) {
    pos->bt_node.left = (struct BinaryTreeNode *) node;
  } else {
    pos->bt_node.right = (struct BinaryTreeNode *) node;
  }
  node->bt_node.parent = (struct BinaryTreeNode *) pos;

  node->diff = 0;
  struct AVLTreeNode *parent = (struct AVLTreeNode *) node->bt_node.parent;
  struct AVLTreeNode *child = node;
  while (parent != NULL) {
    if (parent->bt_node.left == child) {
      --parent->diff;
    } else {
      ++parent->diff;
    }

    if (parent->diff == 0) {
      break;
    } else if (parent->diff > 1) {
      if (child->diff == 1) {
        struct BinaryTreeNode *tmp = (struct BinaryTreeNode *) parent;
        BinaryTree_SingleRotate_Left((struct BinaryTree *) tree, &tmp);
        child->diff = 0;
        parent->diff = 0;
      } else {
        struct AVLTreeNode *tmp = parent;
        BinaryTree_DoubleRotate_Left((struct BinaryTree *) tree, (struct BinaryTreeNode **) &tmp);
        struct AVLTreeNode *right = (struct AVLTreeNode *)(tmp->bt_node.right);
        struct AVLTreeNode *left = (struct AVLTreeNode *)(tmp->bt_node.left);
        if (tmp->diff == 0) {
          right->diff = 0;
          left->diff = 0;
        } else if (tmp->diff == 1) {
          right->diff = 0;
          left->diff = -1;
        } else {
          right->diff = 1;
          left->diff = 0;
        }
      }

      break;
    } else if (parent->diff < -1) {
      if (child->diff == -1) {
        struct BinaryTreeNode *tmp = (struct BinaryTreeNode *) parent;
        BinaryTree_SingleRotate_Right((struct BinaryTree *) tree, &tmp);
        child->diff = 0;
        parent->diff = 0;
      } else {
        struct AVLTreeNode *tmp = parent;
        BinaryTree_DoubleRotate_Right((struct BinaryTree *) tree, (struct BinaryTreeNode **) &tmp);
        struct AVLTreeNode *right = (struct AVLTreeNode *)(tmp->bt_node.right);
        struct AVLTreeNode *left = (struct AVLTreeNode *)(tmp->bt_node.left);
        if (tmp->diff == 0) {
          right->diff = 0;
          left->diff = 0;
        } else if (tmp->diff == 1) {
          right->diff = 0;
          left->diff = -1;
          tmp->diff = 0;
        } else {
          right->diff = 1;
          left->diff = 0;
          tmp->diff = 0;
        }
      }

      break;
    }

    child = parent;
    parent = (struct AVLTreeNode *) parent->bt_node.parent;
  }

  ++tree->size;
  return true;
}

bool AVLTree_Insert(struct AVLTree *tree, uint64_t hash) {
  struct AVLTreeNode *node = AVLTreeNode_Malloc(&AVLTreeNode_pool);
  memset(node, 0, sizeof(*node));
  node->bt_node.hash = hash;
  if (!AVLTree_InsertNode(tree, node)) {
    AVLTreeNode_Free(&AVLTreeNode_pool, node);
    return false;
  }
  return true;
}

bool AVLTree_RemoveNode(struct AVLTree *tree, struct AVLTreeNode *node) {
  return 0;
}
