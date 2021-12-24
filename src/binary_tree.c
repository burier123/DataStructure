#include "binary_tree.h"

#define BinaryTreeNode_MemoryPool_BATCH_SIZE 1024
static const ssize_t BinaryTreeNode_MemoryPool_SINGLE_SIZE = sizeof(struct BinaryTreeNode) > sizeof(void*) ?
    sizeof(struct BinaryTreeNode) : sizeof(void*);

struct BinaryTreeNode_MemoryBatch {
  struct BinaryTreeNode_MemoryBatch *next;

  union {
    struct BinaryTreeNode item;
    struct BinaryTreeNode *ptr;
  } data[BinaryTreeNode_MemoryPool_BATCH_SIZE];
};

struct BinaryTreeNode_MemoryPool {
  ssize_t n_used;
  ssize_t n_free;
  ssize_t capacity;
  struct BinaryTreeNode *free_list_head;
  struct BinaryTreeNode_MemoryBatch *batch_list;
};

static struct BinaryTreeNode_MemoryPool BinaryTreeNode_pool = {0, 0, 0, NULL, NULL};

bool BinaryTreeNode_MemoryBatch_Init(struct BinaryTreeNode_MemoryBatch *block) {
  if (block == NULL) {
    return false;
  }

  block->next = NULL;
  for (int i = 0; i < BinaryTreeNode_MemoryPool_BATCH_SIZE - 1; ++i) {
    block->data[i].ptr = block->data + i + 1;
  }
  block->data[BinaryTreeNode_MemoryPool_BATCH_SIZE - 1].ptr = NULL;

  return true;
}

bool BinaryTreeNode_MemoryPool_Expand(struct BinaryTreeNode_MemoryPool *pool) {
  if (pool == NULL) {
    return false;
  }

  struct BinaryTreeNode_MemoryBatch *new_block = malloc(sizeof(struct BinaryTreeNode_MemoryBatch));
  if (new_block == NULL) {
    return false;
  }

  if (!BinaryTreeNode_MemoryBatch_Init(new_block)) {
    free(new_block);
    return false;
  }

  new_block->data[BinaryTreeNode_MemoryPool_BATCH_SIZE - 1].ptr = pool->free_list_head;
  pool->free_list_head = new_block->data;
  struct BinaryTreeNode_MemoryBatch *tmp = pool->batch_list;
  if (tmp == NULL) {
    pool->batch_list = new_block;
  } else {
    while (tmp->next != NULL) tmp = tmp->next;
    tmp->next = new_block;
  }

  pool->n_free += BinaryTreeNode_MemoryPool_BATCH_SIZE;
  pool->capacity += BinaryTreeNode_MemoryPool_BATCH_SIZE;
  return true;
}

struct BinaryTreeNode_MemoryPool *BinaryTreeNode_MemoryPool_Create() {
  struct BinaryTreeNode_MemoryPool *ret = malloc(sizeof(struct BinaryTreeNode_MemoryPool));
  if (ret == NULL) {
    return NULL;
  }
  memset(ret, 0, sizeof(struct BinaryTreeNode_MemoryPool));

  if (!BinaryTreeNode_MemoryPool_Expand(ret)) {
    free(ret);
    return NULL;
  }

  return ret;
}

struct BinaryTreeNode *BinaryTreeNode_Malloc(struct BinaryTreeNode_MemoryPool *pool) {
  if (pool->free_list_head == NULL && !BinaryTreeNode_MemoryPool_Expand(pool)) {
    return NULL;
  }

  struct BinaryTreeNode *ret = pool->free_list_head;
  pool->free_list_head = *(struct BinaryTreeNode **)pool->free_list_head;
  --pool->n_free;
  ++pool->n_used;
  return ret;
}

void BinaryTreeNode_Free(struct BinaryTreeNode_MemoryPool *pool, struct BinaryTreeNode *del) {
  *(struct BinaryTreeNode **)del = pool->free_list_head;
  pool->free_list_head = del;
  ++pool->n_free;
  --pool->n_used;
}

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
    struct BinaryTreeNode *root = BinaryTreeNode_Malloc(&BinaryTreeNode_pool);
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

  struct BinaryTreeNode *new_node = BinaryTreeNode_Malloc(&BinaryTreeNode_pool);
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

