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

typedef enum SymbolTy {
    FuncTy,
    BoolTy,
    NatTy,
} SymbolTy;

typedef struct Symbol {
    char *ident;
    enum SymbolTy ty;
} Symbol;

Symbol *symbol_new(const char *ident, enum SymbolTy ty);
void symbol_destroy(Symbol *symbol);

typedef struct SymbolTable {
    AVLTree *tree;
} SymbolTable;

SymbolTable *symbol_table_new();
void symbol_table_insert(SymbolTable *table, const char *ident, SymbolTy ty);
SymbolTy symbol_table_find(SymbolTable *table, const char *ident);
void symbol_table_destroy(SymbolTable *table);

#endif  // MINI_COMPILER_SYMBOL_TABLE_H
