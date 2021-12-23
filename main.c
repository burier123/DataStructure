#include "avl_tree.h"
#include "binary_tree.h"

int main (int argc, char **argv) {
  hello();

  struct BinaryTree tree;
  memset(&tree, 0, sizeof(tree));

  BinaryTree_Insert(&tree, 16);
  BinaryTree_Insert(&tree, 56);
  BinaryTree_Insert(&tree, 75);
  BinaryTree_Insert(&tree, 3);
  BinaryTree_Insert(&tree, 5);
  BinaryTree_Insert(&tree, 8);
  BinaryTree_Insert(&tree, 18);
  BinaryTree_Insert(&tree, 69);
  BinaryTree_Insert(&tree, 55);
  BinaryTree_Insert(&tree, 1);

  return 0;
}
