#include "avl_tree.h"
#include "binary_tree.h"

int main (int argc, char **argv) {
  hello();

  struct BinaryTree tree;
  memset(&tree, 0, sizeof(tree));

  uint64_t numbers[4096];
  uint64_t tmp;
  for (int i = 0; i < 4096; ++i) {
    tmp = rand();
    numbers[i] = tmp;
    BinaryTree_Insert(&tree, tmp);
  }

  BinaryTree_Print_MidOrder(tree.root);
  if (!BinaryTree_Check(tree.root)) {
    printf("Check failed!");
  }

  for (int i = 0; i < 1024; ++i) {
    BinaryTree_Remove(&tree, numbers[rand() % 4096]);
    if (!BinaryTree_Check(tree.root)) {
      printf("Check failed!");
    }
  }

  return 0;
}
