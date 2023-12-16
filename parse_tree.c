#include "parse_tree.h"

ParseTree* parse_tree_init(ParseTreeNode* node) {
    ParseTree* tree = malloc(sizeof(ParseTree));
    tree->root;
    return tree;
}

SLRSymbol* slr_symbol_init_nt(NonTerminal nt) {
    SLRSymbol* sym = malloc(sizeof(SLRSymbol));
    sym->nt = nt;
    return sym;
}

ParseTreeNode* parse_tree_node_init(SLRSymbol* sym) {
    ParseTreeNode* node = malloc(sizeof(ParseTreeNode));
    node->SLRSymbol = sym;
    node->children = NULL;
    return node;
}

void parse_tree_node_add_first(ParseTreeNode* node, ParseTreeNode* child) {
    if (node->children == NULL) {
        cc_deque_new(&node->children);
    }
    cc_deque_add_first(node->children, child);
}

void parse_tree_node_add_last(ParseTreeNode* node, ParseTreeNode* child) {
    if (node->children == NULL) {
        cc_deque_new(&node->children);
    }
    cc_deque_add_last(node->children, child);
}

ParseTreeNode* parse_tree_node_remove_last(ParseTreeNode* node) {
    ParseTreeNode* out;
    if (node->children == NULL || cc_deque_size(node->children) == 0) {
        return NULL;
    } else {
        cc_deque_remove_last(node->children, (void*)&out);
        return out;
    }
}