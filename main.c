#include "avl_tree.h"
#include "binary_tree.h"
#include "rb_tree.h"
#include "dict.h"

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

void DictTest(void) {
  struct Dict *dict = Dict_Create();
  if (dict == NULL) {
    printf("Create dict failed!");
    return;
  }

  Dict_Insert(dict, 127);
  Dict_Insert(dict, 78537785);
  Dict_Insert(dict, 122745277);
  Dict_Insert(dict, 1242717);
  Dict_Insert(dict, 78);
  Dict_Insert(dict, 9649649);
  Dict_Insert(dict, 123123333);
  Dict_Insert(dict, 3233333);
  Dict_Insert(dict, 965342);
  Dict_Insert(dict, 568473);
  Dict_Insert(dict, 8588558);
  Dict_Insert(dict, 1636661);
  Dict_Insert(dict, 774257);
  Dict_Insert(dict, 52522);
  Dict_Insert(dict, 6374);
  Dict_Insert(dict, 8454);
  for (int i = 0; i < 9996; ++i) {
    if (!Dict_Insert(dict, i)) {
      printf("Dict insert failed at:%d\n", i);
    }
  }

  Dict_Remove(dict, 1636661);
  Dict_Remove(dict, 774257);
  Dict_Remove(dict, 52522);
  Dict_Remove(dict, 6374);
  Dict_Remove(dict, 8454);

  printf("dict find %d, res: %d\n", 1636661, (Dict_Find(dict, 1636661) != NULL));
  printf("dict find %d, res: %d\n", 8454, (Dict_Find(dict, 8454) != NULL));
  printf("dict find %d, res: %d\n", 123123333, (Dict_Find(dict, 123123333) != NULL));
  printf("dict find %d, res: %d\n", 5656, (Dict_Find(dict, 5656) != NULL));
  printf("dict find %d, res: %d\n", 123, (Dict_Find(dict, 123) != NULL));
}

int main (int argc, char **argv) {
  //AVLTreeTest();
  //RBTreeTest();
  DictTest();

  return 0;
}
