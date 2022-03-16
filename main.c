#include "avl_tree.h"
#include "binary_tree.h"
#include "rb_tree.h"

void AVLTreeTest(void) {
  struct AVLTree tree;
  memset(&tree, 0, sizeof(tree));

  uint64_t numbers[4096];
  struct AVLTreeNode *nodes[4096];
  uint64_t tmp;
  int height;
  for (int i = 0; i < 4096; ++i) {
    tmp = rand();
    numbers[i] = tmp;
    nodes[i] = AVLTree_Insert(&tree, tmp);
  }

  BinaryTree_Print_MidOrder(tree.root);
  if (!BinaryTree_Check(tree.root)) {
    printf("Check failed!\n");
  }

  if (!AVLTree_CheckBalance(tree.root, &height)) {
    printf("AVLTree balance check failed! height:%d\n", height);
  } else {
    printf("AVLTree balance check passed! height:%d\n", height);
  }

  for (int i = 0; i < 2048; ++i) {
    AVLTree_RemoveNode(&tree, nodes[i]);
    if (!BinaryTree_Check(tree.root)) {
      printf("Check failed!\n");
    }
    if (!AVLTree_CheckBalance(tree.root, &height)) {
      printf("AVLTree balance check failed! height:%d\n", height);
    }
  }

  if (!BinaryTree_Check(tree.root)) {
    printf("Check failed!\n");
  }

  for (int i = 0; i < 1024; ++i) {
    AVLTree_InsertNode(&tree, nodes[i]);
    if (!BinaryTree_Check(tree.root)) {
      printf("Check failed!\n");
    }
    if (!AVLTree_CheckBalance(tree.root, &height)) {
      printf("AVLTree balance check failed! height:%d\n", height);
    }
  }


  AVLTree_Insert(&tree, 3);
  AVLTree_Insert(&tree, 2);
  AVLTree_Insert(&tree, 1);
  AVLTree_Insert(&tree, 4);
  AVLTree_Insert(&tree, 5);
  AVLTree_Insert(&tree, 6);
  AVLTree_Insert(&tree, 7);
  AVLTree_Insert(&tree, 16);
  AVLTree_Insert(&tree, 15);
  AVLTree_Insert(&tree, 14);
  struct AVLTreeNode *test = AVLTree_Insert(&tree, 13);
  AVLTree_Insert(&tree, 12);
  AVLTree_Insert(&tree, 11);
  AVLTree_Insert(&tree, 10);
  AVLTree_Insert(&tree, 8);

  AVLTree_RemoveNode(&tree, test);
}

void RBTreeTest(void) {
  struct RBTree tree;
  memset(&tree, 0, sizeof(tree));

  struct RBTreeNode *nodes[4096];
  int n;
  for (int i = 0; i < 4096; ++i) {
    nodes[i] = RBTree_Insert(&tree, rand());
    if (!BinaryTree_Check(tree.root)) {
      printf("Check failed!\n");
    }
    if (!RBTree_Check(tree.root, &n)) {
      printf("RBTree balance check failed! height:%d\n", n);
    }
  }
}

int main (int argc, char **argv) {
  AVLTreeTest();
  RBTreeTest();

  return 0;
}
