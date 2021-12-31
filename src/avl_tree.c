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
  if (BinaryTree_InternalFindPosition((struct BinaryTreeNode *) tree->root,
                                      node->bt_node.hash,
                                      (struct BinaryTreeNode **) &pos)) {
    return false;
  }

  if (pos->bt_node.hash > node->bt_node.hash) {
    pos->bt_node.left = (struct BinaryTreeNode *) node;
  } else {
    pos->bt_node.right = (struct BinaryTreeNode *) node;
  }
  node->bt_node.parent = (struct BinaryTreeNode *) pos;

  node->diff = 0;
  node->bt_node.left = NULL;
  node->bt_node.right = NULL;
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
        struct AVLTreeNode *right = (struct AVLTreeNode *) (tmp->bt_node.right);
        struct AVLTreeNode *left = (struct AVLTreeNode *) (tmp->bt_node.left);
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
    } else if (parent->diff < -1) {
      if (child->diff == -1) {
        struct BinaryTreeNode *tmp = (struct BinaryTreeNode *) parent;
        BinaryTree_SingleRotate_Right((struct BinaryTree *) tree, &tmp);
        child->diff = 0;
        parent->diff = 0;
      } else {
        struct AVLTreeNode *tmp = parent;
        BinaryTree_DoubleRotate_Right((struct BinaryTree *) tree, (struct BinaryTreeNode **) &tmp);
        struct AVLTreeNode *right = (struct AVLTreeNode *) (tmp->bt_node.right);
        struct AVLTreeNode *left = (struct AVLTreeNode *) (tmp->bt_node.left);
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

struct AVLTreeNode* AVLTree_Insert(struct AVLTree *tree, uint64_t hash) {
  struct AVLTreeNode *node = AVLTreeNode_Malloc(&AVLTreeNode_pool);
  memset(node, 0, sizeof(*node));
  node->bt_node.hash = hash;
  if (!AVLTree_InsertNode(tree, node)) {
    AVLTreeNode_Free(&AVLTreeNode_pool, node);
    return NULL;
  }
  return node;
}

bool AVLTree_RemoveNode(struct AVLTree *tree, struct AVLTreeNode *node) {
  if (tree == NULL || tree->size == 0 || node == NULL) {
    return false;
  }

  struct AVLTreeNode *target = NULL;
  if (!BinaryTree_InternalFindPosition(tree->root, node->bt_node.hash, &target) || target != node) {
    return false;
  }

  struct BinaryTreeNode **p = BinaryTree_GetParentUpdatePosition(tree, target);
  struct AVLTreeNode *parent, *child;

  if (target->bt_node.left == NULL) {
    if (target->bt_node.right == NULL) {
      parent = (struct AVLTreeNode *) target->bt_node.parent;
      child = NULL;
      *p = NULL;
    } else {
      *p = target->bt_node.right;
      target->bt_node.right->parent = target->bt_node.parent;
      parent = (struct AVLTreeNode *) target->bt_node.parent;
      child = (struct AVLTreeNode *) target->bt_node.right;
    }
  } else if (target->bt_node.right == NULL) {
    *p = target->bt_node.left;
    target->bt_node.left->parent = target->bt_node.parent;
    parent = (struct AVLTreeNode *) target->bt_node.parent;
    child = (struct AVLTreeNode *) target->bt_node.left;
  } else {
    struct BinaryTreeNode *sub;
    if (target->diff == -1) {
      if (BinaryTree_FindMax(target->bt_node.left, &sub)) {
        if (target->bt_node.left != sub) {
          parent = (struct AVLTreeNode *) sub->parent;
          child = (struct AVLTreeNode *) sub->left;
          sub->parent->right = sub->left;
          if (sub->left != NULL) {
            sub->left->parent = sub->parent;
          }
          sub->parent = target->bt_node.parent;
          *p = sub;
          sub->left = target->bt_node.left;
          sub->right = target->bt_node.right;
          target->bt_node.left->parent = sub;
          target->bt_node.right->parent = sub;
        } else {
          parent = (struct AVLTreeNode *) sub;
          child = (struct AVLTreeNode *) sub->left;
          sub->parent = target->bt_node.parent;
          *p = sub;
          sub->right = target->bt_node.right;
          target->bt_node.right->parent = sub;
        }
        ((struct AVLTreeNode *) sub)->diff = target->diff;
      } else {
        return false;
      }
    } else {
      if (BinaryTree_FindMin(target->bt_node.right, &sub)) {
        if (target->bt_node.right != sub) {
          parent = (struct AVLTreeNode *) sub->parent;
          child = (struct AVLTreeNode *) sub->right;
          sub->parent->left = sub->right;
          if (sub->right != NULL) {
            sub->right->parent = sub->parent;
          }
          sub->parent = target->bt_node.parent;
          *p = sub;
          sub->left = target->bt_node.left;
          sub->right = target->bt_node.right;
          target->bt_node.left->parent = sub;
          target->bt_node.right->parent = sub;
        } else {
          parent = (struct AVLTreeNode *) sub;
          child = (struct AVLTreeNode *) sub->right;
          sub->parent = target->bt_node.parent;
          *p = sub;
          sub->left = target->bt_node.left;
          target->bt_node.left->parent = sub;
        }
        ((struct AVLTreeNode *) sub)->diff = target->diff;
      } else {
        return false;
      }
    }
  }

  while (parent != NULL) {
    if (parent->bt_node.left == child && parent->bt_node.right != NULL) {
      ++parent->diff;
    } else if (parent->bt_node.right == child && parent->bt_node.left != NULL) {
      --parent->diff;
    } else {
      parent->diff = 0;
    }

    if (parent->diff > 1) {
      if (child == NULL || child->diff == 0) {
        child = (struct AVLTreeNode *) parent->bt_node.right;
      }

      if (child->diff == -1) {
        struct AVLTreeNode *tmp = parent;
        BinaryTree_DoubleRotate_Left((struct BinaryTree *) tree, (struct BinaryTreeNode **) &tmp);
        struct AVLTreeNode *right = (struct AVLTreeNode *) (tmp->bt_node.right);
        struct AVLTreeNode *left = (struct AVLTreeNode *) (tmp->bt_node.left);
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
        parent = (struct AVLTreeNode *) tmp;
      } else {
        struct BinaryTreeNode *tmp = (struct BinaryTreeNode *) parent;
        child = (struct AVLTreeNode *) parent->bt_node.right;
        BinaryTree_SingleRotate_Left((struct BinaryTree *) tree, &tmp);
        if (child->diff == 1) {
          child->diff = 0;
          parent->diff = 0;
        } else {
          child->diff = -1;
          parent->diff = 1;
          break;
        }
        parent = (struct AVLTreeNode *) tmp;
      }
    } else if (parent->diff < -1) {
      if (child == NULL || child->diff == 0) {
        child = (struct AVLTreeNode *) parent->bt_node.left;
      }

      if (child->diff == 1) {
        struct AVLTreeNode *tmp = parent;
        BinaryTree_DoubleRotate_Right((struct BinaryTree *) tree, (struct BinaryTreeNode **) &tmp);
        struct AVLTreeNode *right = (struct AVLTreeNode *) (tmp->bt_node.right);
        struct AVLTreeNode *left = (struct AVLTreeNode *) (tmp->bt_node.left);
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
        parent = (struct AVLTreeNode *) tmp;
      } else {
        struct BinaryTreeNode *tmp = (struct BinaryTreeNode *) parent;
        child = (struct AVLTreeNode *) parent->bt_node.left;
        BinaryTree_SingleRotate_Right((struct BinaryTree *) tree, &tmp);
        if (child->diff == -1) {
          child->diff = 0;
          parent->diff = 0;
        } else {
          child->diff = 1;
          parent->diff = -1;
          break;
        }
        parent = (struct AVLTreeNode *) tmp;
      }
    } else if (parent->diff == 1 || parent->diff == -1) {
      break;
    }

    child = parent;
    parent = (struct AVLTreeNode *) parent->bt_node.parent;
  }

  --tree->size;
  return true;
}

bool AVLTree_CheckBalance(struct AVLTreeNode *node, int *height) {
  if (node == NULL) {
    *height = 0;
    return true;
  }

  int rh, lh;
  if (!AVLTree_CheckBalance((struct AVLTreeNode *) node->bt_node.left, &lh)) {
    return false;
  }

  if (!AVLTree_CheckBalance((struct AVLTreeNode *) node->bt_node.right, &rh)) {
    return false;
  }

  if (lh + node->diff != rh) {
    printf("Check balance failed at node:%p\n", node);
    return false;
  }

  *height = lh > rh ? lh + 1 : rh + 1;
  return true;
}
