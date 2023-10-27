#include "symbol_table.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* AVL Tree impl */
static Node *newNode(Key k);
static Node *minNode(Node *root);
static Node *deleteMin(Node *root);
static int height(Node *root);
static void updateHeight(Node *root);
static Node *rotateLeft(Node **root);
static Node *rotateRight(Node **root);
static Node *balance(Node **proot);
static Node *insertNodeHelper(Node **proot, Key k, int (*compare)(Key, Key));
static Node *findNodeHelper(Node *root, Key k, int (*compare)(Key, Key));
static Node *deleteNodeHelper(Node **proot, Key k, int (*compare)(Key, Key));
static void destroyTreeHelper(Node *root, void (*destroy)(Key k));

/* Comparator and Destructor */
static int symbolComparator(Key l, Key r);
static void symbolDestructor(Key k);

/**
 * Create a new node
 *
 * @param k - node key
 * @return the pointer to the new node
 *
 */
Node *newNode(Key k) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->key = k;
    node->height = 0;
    return node;
}

/**
 * Find the node who has minimum value in the tree
 *
 * @param root - pointer to the tree root
 * @return the pointer to the minNode
 *
 */
Node *minNode(Node *root) {
    if (root->left == NULL) return root;
    return minNode(root->left);
}

/**
 * Delete the node who has minimum value in the tree
 *
 * @param root - pointer to the tree root
 * @return the pointer to the tree root
 *
 */
Node *deleteMin(Node *root) {
    if (root->left == NULL) return root->right;
    root->left = deleteMin(root->left);
    updateHeight(root);
    balance(&root);
    return root;
}

/**
 * Height of current node
 *
 * @param root - pointer to the node
 * @return height
 *
 */
int height(Node *root) { return root == NULL ? -1 : root->height; }

/**
 * Update the height of current node
 *
 * @param root - pointer to the node
 *
 */
void updateHeight(Node *root) {
    if (root == NULL) return;
    int left = root->left == NULL ? -1 : root->left->height;
    int right = root->right == NULL ? -1 : root->right->height;
    root->height = (left > right ? left : right) + 1;
}

/**
 * Make a left rotation to a node
 *
 * @param proot - pointer to the node to be rotated
 * @return the new root after rotation
 *
 */
Node *rotateLeft(Node **proot) {
    Node *root = *proot;
    *proot = root->right;
    root->right = (*proot)->left;
    (*proot)->left = root;
    updateHeight(root);
    updateHeight(*proot);
    return *proot;
}

/**
 * Make a right rotation to a node
 *
 * @param proot - pointer to the node to be rotated
 * @return the new root after rotation
 *
 */
Node *rotateRight(Node **proot) {
    Node *root = *proot;
    *proot = root->left;
    root->left = (*proot)->right;
    (*proot)->right = root;
    updateHeight(root);
    updateHeight(*proot);
    return *proot;
}

/**
 * Make AVL tree balanced after insertion
 *
 * @param proot - pointer to the node to be balanced
 * @return the new root after balancing
 *
 */
Node *balance(Node **proot) {
    if (proot == NULL || *proot == NULL) {
        return NULL;
    }
    Node *root = *proot;
    if (height(root->left) - height(root->right) > 1)  // left heavy
    {
        root = root->left;
        if (height(root->left) < height(root->right))  // right heavy
            (*proot)->left = rotateLeft(&root);
        rotateRight(proot);
    } else if (height(root->right) - height(root->left) > 1)  // right heavy
    {
        root = root->right;
        if (height(root->left) > height(root->right))  // left heavy
            (*proot)->right = rotateRight(&root);
        rotateLeft(proot);  // right heavy
    } else
        ;  // balanced, do nothing
    return *proot;
}

/**
 * Insert a node to AVL tree (Helper)
 * SAFETY: caller is responsible to manage the memory of key k
 *
 * @param proot - pointer to the tree root
 * @param k - key to be inserted
 * @param compare - key comparator
 * @return the pointer to inserted node
 *
 */
Node *insertNodeHelper(Node **proot, Key k, int (*compare)(Key, Key)) {
    if (*proot == NULL) {
        *proot = newNode(k);
        return *proot;
    }
    Node *root = *proot;
    Node *newNode = NULL;
    if (compare(root->key, k) < 0)
        newNode = insertNodeHelper(&(root->right), k, compare);
    else if (compare(root->key, k) > 0)
        newNode = insertNodeHelper(&(root->left), k, compare);
    else
        return NULL;
    balance(proot);
    updateHeight(root);
    return newNode;
}

/**
 * Find a node in AVL tree (Helper)
 * SAFETY: caller is responsible to manage the memory of key k
 *
 * @param root - pointer to the tree root
 * @param k - key to be searched
 * @param compare - key comparator
 * @return the pointer to node, if nothing found return null
 *
 */
Node *findNodeHelper(Node *root, Key k, int (*compare)(Key, Key)) {
    Node *cur = root;
    while (cur != NULL) {
        if (compare(cur->key, k) < 0) {
            // search in the right tree
            cur = cur->right;
        } else if (compare(cur->key, k) > 0) {
            // search in the left tree
            cur = cur->left;
        } else {
            break;
        }
    }
    return cur;
}

/**
 * Delete a node in AVL tree (Helper)
 * SAFETY: caller is responsible to manage the memory of key k
 * and the node that callee returns.
 *
 * @param proot - pointer to the tree root
 * @param k - key to be deleted
 * @param compare - key comparator
 * @return the pointer to deleted node, returns NULL if no such node exists
 *
 */
Node *deleteNodeHelper(Node **proot, Key k, int (*compare)(Key, Key)) {
    if (*proot == NULL) return NULL;
    Node *root = *proot;
    if (compare(root->key, k) > 0)
        root = deleteNodeHelper(&(root->left), k, compare);
    else if (compare(root->key, k) < 0)
        root = deleteNodeHelper(&(root->right), k, compare);
    else {
        // one child cases and no child cases
        if (root->right == NULL) {
            *proot = root->left;
            updateHeight(*proot);
            return root;
        }
        if (root->left == NULL) {
            *proot = root->right;
            updateHeight(*proot);
            return root;
        }
        // two child cases
        *proot = minNode(root->right);
        (*proot)->right = deleteMin(root->right);
        (*proot)->left = root->left;
    }
    updateHeight(*proot);
    balance(proot);
    return root;
}

/**
 * Destroy a AVL tree
 *
 * @param root - pointer to the tree root
 * @param destroy - key destructor
 *
 */
void destroyTreeHelper(Node *root, void (*destroy)(Key k)) {
    if (root == NULL) return;
    destroyTreeHelper(root->left, destroy);
    destroyTreeHelper(root->right, destroy);
    destroy(root->key);
    free(root);
}

/**
 * Create an AVLTree with comparator and destructor
 * SAFETY: caller is responsible to manage the memory of key k
 *
 * @param compare - pointer to the comparator
 * @param destroy - pointer to the destructor
 * @return the pointer to the empty AVLTree,
 * return null if comparator or destructor is not given
 *
 */
AVLTree *createTree(int (*compare)(Key, Key), void (*destroy)(Key k)) {
    if (compare == NULL || destroy == NULL) return NULL;
    AVLTree *t = (AVLTree *)malloc(sizeof(AVLTree));
    t->compare = compare;
    t->destroy = destroy;
    t->root = NULL;
    return t;
}

/**
 * Insert a node to AVL tree
 * SAFETY: caller is responsible to manage the memory of key k
 *
 * @param tree - pointer to the AVLtree
 * @param k - key to be inserted
 * @return the pointer to inserted Key
 *
 */
Key *insertNode(AVLTree *tree, Key k) {
    Node *inserted = insertNodeHelper(&(tree->root), k, tree->compare);
    return inserted ? inserted->key : NULL;
}

/**
 * Find a node in AVL tree
 * SAFETY: caller is responsible to manage the memory of key k
 *
 * @param tree - pointer to the AVLtree
 * @param k - key to be searched
 * @return the pointer to key, if nothing found return null
 *
 */
Key *findNode(AVLTree *tree, Key k) {
    Node *find = findNodeHelper(tree->root, k, tree->compare);
    return find ? find->key : NULL;
}

/**
 * Delete a node in AVL tree
 * SAFETY: caller is responsible to manage the memory of key k
 * and the key that callee returns.
 *
 * @param proot - pointer to the tree root
 * @param k - key to be deleted
 * @return the pointer to deleted key, returns NULL if no such node exists
 *
 */
Key *deleteNode(AVLTree *tree, Key k) {
    Node *deleted = deleteNodeHelper(&(tree->root), k, tree->compare);
    Key data = deleted ? deleted->key : NULL;
    free(deleted);
    return data;
}

/**
 * Destroy an AVL Tree
 *
 * @param tree - pointer to the AVL Tree
 *
 */
void destroyTree(AVLTree *tree) {
    destroyTreeHelper(tree->root, tree->destroy);
    free(tree);
}