#include "parse_tree.h"

ParseTree* parse_tree_init(ParseTreeNode* node) {
    ParseTree* tree = malloc(sizeof(ParseTree));
    tree->root = node;
    return tree;
}

SLRSymbol* slr_symbol_init_nt(NonTerminal nt) {
    SLRSymbol* sym = malloc(sizeof(SLRSymbol));
    sym->nt = nt;
    return sym;
}

ParseTreeNode* parse_tree_node_init(SLRSymbol* sym, NodeTy ty) {
    ParseTreeNode* node = malloc(sizeof(ParseTreeNode));
    node->SLRSymbol = sym;
    node->ty = ty;
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

void destroy_parse_tree_node(ParseTreeNode* node) {
    if (node->SLRSymbol != NULL) {
        switch (node->ty) {
            case NODE_TERMINAL:
                destroy_token(node->SLRSymbol->token);
                free(node->SLRSymbol);
                break;
            case NODE_NON_TERMINAL:
                free(node->SLRSymbol);
                break;
        }
    }

    if (node->children != NULL) {
        for (size_t i = 0; i < cc_deque_size(node->children); i++) {
            ParseTreeNode* child;
            cc_deque_get_at(node->children, i, (void*)&child);
            destroy_parse_tree_node(child);
        }
    }
    free(node);
}

void destroy_parse_tree(ParseTree* tree) {
    destroy_parse_tree_node(tree->root);
    free(tree);
}
