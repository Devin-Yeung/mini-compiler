#ifndef MINI_COMPILER_PARSE_TREE_H
#define MINI_COMPILER_PARSE_TREE_H

#include "parser.h"

typedef enum NodeTy {
    NODE_TERMINAL,
    NODE_NON_TERMINAL,
} NodeTy;

typedef struct ParseTreeNode {
    SLRSymbol* SLRSymbol;
    CC_Deque* children;
} ParseTreeNode;

typedef struct ParseTree {
    ParseTreeNode* root;
} ParseTree;

ParseTree* parse_tree_init(ParseTreeNode* node);
ParseTreeNode* parse_tree_node_init(SLRSymbol* sym);
void parse_tree_node_add_first(ParseTreeNode* node, ParseTreeNode* child);
ParseTreeNode* parse_tree_node_remove_last(ParseTreeNode* node);

#endif  // MINI_COMPILER_PARSE_TREE_H
