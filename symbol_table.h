#ifndef MINI_COMPILER_SYMBOL_TABLE_H
#define MINI_COMPILER_SYMBOL_TABLE_H

typedef void *Key;

typedef struct Node {
    Key key;
    int height;
    struct Node *left, *right;
} Node;

typedef struct AVLTree {
    Node *root;
    /**
     * Compare two keys
     * - if l = r return 0
     * - if l > r return positive number
     * - if l < r return negative number
     *
     * @param l left operand
     * @param r right operand
     * @return a number to represent who is larger
     *
     */
    int (*compare)(Key l, Key r);
    /**
     * Key destructor
     *
     * @param k key itself
     *
     */
    void (*destroy)(Key k);
} AVLTree;

AVLTree *createTree(int (*compare)(Key, Key), void (*destroy)(Key k));
Key *insertNode(AVLTree *tree, Key k);
Key *findNode(AVLTree *tree, Key k);
Key *deleteNode(AVLTree *tree, Key k);
void destroyTree(AVLTree *tree);

#endif  // MINI_COMPILER_SYMBOL_TABLE_H
