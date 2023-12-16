#include "parser.h"

#include <assert.h>  // TODO: remove me in release
#include <stdarg.h>
#include <stdio.h>

#ifdef LOG
#include "log.h"
#else
// If log is not enable, just ignore the macro call
#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_fatal(...)
#endif

char *to_string(const char *str) {
    if (str == NULL) {
        return NULL;
    }

    size_t length = strlen(str);
    // Allocate memory for the new string, including space for the null
    // terminator
    char *ret = (char *)malloc((length + 1) * sizeof(char));

    // Copy the input string to the newly allocated memory
    strcpy(ret, str);

    return ret;
}

unsigned log10u(unsigned n) {
    unsigned ret = 0;
    while (n > 0) {
        n /= 10;
        ret++;
    }
    return ret;
}

Grammar *grammar_new() {
    Grammar *g = malloc(sizeof(Grammar));

    g->prods[0] = P0;
    g->prods[1] = P1;
    g->prods[2] = P2;
    g->prods[3] = P3;
    g->prods[4] = P4;
    g->prods[5] = P5;
    g->prods[6] = P6;
    g->prods[7] = P7;
    g->prods[8] = P8;
    g->prods[9] = P9;
    g->prods[10] = P10;
    g->prods[11] = P11;
    g->prods[12] = P12;
    g->prods[13] = P13;
    g->prods[14] = P14;
    g->prods[15] = P15;
    g->prods[16] = P16;
    g->prods[17] = P17;
    g->prods[18] = P18;

    return g;
}

void destroy_grammar(Grammar *g) { free(g); }

SLRItem *slr_item_token(Token *tok, SLRSymbolTy ty, unsigned value) {
    SLRItem *item = malloc(sizeof(SLRItem));

    SLRSymbol *sym = malloc(sizeof(SLRSymbol));
    sym->token = tok;

    item->symbol = sym;
    item->ty = ty;
    item->value = value;
    return item;
}

SLRItem *slr_item_nt(NonTerminal nt, SLRSymbolTy ty, unsigned value) {
    SLRItem *item = malloc(sizeof(SLRItem));

    SLRSymbol *sym = malloc(sizeof(SLRSymbol));
    sym->nt = nt;

    item->symbol = sym;
    item->ty = ty;
    item->value = value;
    return item;
}

SLRParser *slr_parser_init(Grammar *grammar, const SLRTable *table) {
    SLRParser *parser = malloc(sizeof(SLRParser));

    parser->grammar = grammar;
    parser->table = table;
    cc_deque_new(&parser->stack);
    SLRItem *item = slr_item_token(NULL, SLR_SYMBOL_VOID, 0);
    cc_deque_add(parser->stack, item);
    parser->trace = slr_trace_init();
    cc_deque_add_last(parser->trace->stack_trace,
                      (void *)stringify_slr_stack(parser));
    cc_deque_add_last(parser->trace->op_trace,
                      (void *)calloc(1, sizeof(char)));  // empty string
    parser->parse_tree = parse_tree_node_init(NULL, NODE_NON_TERMINAL);
    return parser;
}

void deep_destroy_slr_item(SLRItem *item) {
    if (item->ty == SLR_SYMBOL_TOKEN) {
        destroy_token(item->symbol->token);
    }
    free(item->symbol);
    free(item);
}

void shallow_destroy_slr_item(SLRItem *item) { free(item); }

void destroy_slr_item_cb(void *item) { deep_destroy_slr_item((SLRItem *)item); }

void destroy_slr_parser(SLRParser *parser) {
    if (parser->parse_tree != NULL) {
        destroy_parse_tree_node(parser->parse_tree);
    }
    cc_deque_destroy_cb(parser->stack, destroy_slr_item_cb);
    destroy_slr_trace(parser->trace);
    free(parser);
}

ParserState slr_parser_step(SLRParser *parser, Token *tok) {
    char *buf;
    log_debug("Step(%s)", tok->lexeme);
    SLRItem *last = NULL;
    cc_deque_get_last(parser->stack, (void *)&last);
    SLRop next = shift_reduce_table_get(parser->table->shift_reduce_table,
                                        last->value, tok->ty);
    if (next.ty == SLR_SHIFT) {
        log_debug("(Shift, %d)", next.value);
        SLRItem *item = slr_item_token(tok, SLR_SYMBOL_TOKEN, next.value);
        cc_deque_add_last(parser->stack, (void *)item);
        // Display the SLR Stack
        cc_deque_add_last(parser->trace->stack_trace,
                          (void *)stringify_slr_stack(parser));
        cc_deque_add_last(parser->trace->op_trace,
                          (void *)stringify_slr_op(&next, parser->grammar));
    } else if (next.ty == SLR_REDUCE) {
        log_debug("(Reduce, %d)", next.value);
        if (next.value == 0) {
            log_debug("Accept!");
            SLRItem *item = slr_item_token(tok, SLR_SYMBOL_TOKEN, 0);
            cc_deque_add_last(parser->stack, (void *)item);
            cc_deque_add_last(parser->trace->stack_trace,
                              (void *)stringify_slr_stack(parser));
            cc_deque_add_last(parser->trace->op_trace,
                              (void *)to_string("accept"));
            return PARSER_ACCEPT;
        }
        Production prod = parser->grammar->prods[next.value];
        // Consume all the rhs item and create a new parse tree node
        ParseTreeNode *node = parse_tree_node_init(
            slr_symbol_init_nt(prod.lhs.value.nt), NODE_NON_TERMINAL);
        for (int i = (int)(prod.n_rhs - 1); i >= 0; i--) {
            if (cc_deque_remove_last(parser->stack, (void *)&last) == CC_OK) {
                // check if production rule is matched
                // Terminal
                if (last->ty == SLR_SYMBOL_TOKEN) {
                    if (prod.rhs[i].ty == TERM_TERMINAL &&
                        prod.rhs[i].value.t == last->symbol->token->ty) {
                        // TODO: Remove this line if the ownership is
                        // TODO: transferred to the parse tree
                        parse_tree_node_add_first(
                            node,
                            parse_tree_node_init(last->symbol, NODE_TERMINAL));
                    } else {
                        deep_destroy_slr_item(last);
                        return PARSER_REJECT;
                    }
                }

                // Non-terminal
                if (last->ty == SLR_SYMBOL_NON_TERMINAL) {
                    if (prod.rhs[i].ty == TERM_NON_TERMINAL &&
                        prod.rhs[i].value.nt == last->symbol->nt) {
                        ParseTreeNode *nt_node =
                            parse_tree_node_remove_last(parser->parse_tree);
                        parse_tree_node_add_first(node, nt_node);
                        free(last->symbol);  // ugly patch to fix mem leak
                    } else {
                        deep_destroy_slr_item(last);
                        return PARSER_REJECT;
                    }
                }
                // shallow destroy since the token will be taken by parse tree
                shallow_destroy_slr_item(last);
            } else {
                // Do not have enough item to reduce a production rule
                return PARSER_REJECT;
            }
        }
        // Node construction is done, push to the tree
        parse_tree_node_add_last(parser->parse_tree, node);
        // Push the lhs item to the stack
        cc_deque_get_last(parser->stack, (void *)&last);
        SLRop op = goto_table_get(parser->table->goto_table, last->value,
                                  prod.lhs.value.nt);
        SLRItem *item =
            slr_item_nt(prod.lhs.value.nt, SLR_SYMBOL_NON_TERMINAL, op.value);
        log_debug("(GOTO, %d)", op.value);
        cc_deque_add_last(parser->stack, (void *)item);
        // Display the SLR Stack
        cc_deque_add_last(parser->trace->stack_trace,
                          (void *)stringify_slr_stack(parser));
        cc_deque_add_last(parser->trace->op_trace,
                          (void *)stringify_slr_op(&next, parser->grammar));
        // deal with the incoming tok
        return slr_parser_step(parser, tok);
    } else /* Empty Cell, Reject */ {
        log_debug("Reject due to the empty cell");
        SLRItem *item = slr_item_token(tok, SLR_SYMBOL_TOKEN, 0);
        cc_deque_add_last(parser->stack, (void *)item);
        cc_deque_add_last(parser->trace->stack_trace,
                          (void *)stringify_slr_stack(parser));
        cc_deque_add_last(parser->trace->op_trace,
                          (void *)to_string("syntax error"));
        return PARSER_REJECT;
    }
    return PARSER_IDLE;
}

SLRop shift_reduce_table_get(const SLRop (*shift_reduce_table)[16],
                             unsigned state_id, TokenTy ty) {
    // mapping a terminal to table column index
    unsigned symbol_idx = 0;
    switch (ty) {
        case Semicolon:
            symbol_idx = 0;
            break;
        case FuncDecl:
            symbol_idx = 1;
            break;
        case Identifier:
            symbol_idx = 2;
            break;
        case Arrow:
            symbol_idx = 3;
            break;
        case NatDecl:
            symbol_idx = 4;
            break;
        case BoolDecl:
            symbol_idx = 5;
            break;
        case QuestionMark:
            symbol_idx = 6;
            break;
        case Colon:
            symbol_idx = 7;
            break;
        case LeftParen:
            symbol_idx = 8;
            break;
        case RightParen:
            symbol_idx = 9;
            break;
        case Literal:
            symbol_idx = 10;
            break;
        case Plus:
            symbol_idx = 11;
            break;
        case Ampersand:
            symbol_idx = 12;
            break;
        case Less:
            symbol_idx = 13;
            break;
        case Equal:
            symbol_idx = 14;
            break;
        case Eof:
            symbol_idx = 15;
            break;
        default:
            __builtin_unreachable();
    }
    log_debug("Checking Shift-Reduce Table [%d, %d]", state_id, symbol_idx);
    return shift_reduce_table[state_id][symbol_idx];
}

SLRop goto_table_get(const SLRop (*goto_table)[9], unsigned state_id,
                     NonTerminal nt) {
    // mapping a non-terminal to table column index
    unsigned symbol_idx = 0;
    switch (nt) {
        case 'S':
            symbol_idx = 0;
            break;
        case 'F':
            symbol_idx = 1;
            break;
        case 'A':
            symbol_idx = 2;
            break;
        case 'T':
            symbol_idx = 3;
            break;
        case 'C':
            symbol_idx = 4;
            break;
        case 'E':
            symbol_idx = 5;
            break;
        case 'I':
            symbol_idx = 6;
            break;
        case 'B':
            symbol_idx = 7;
            break;
        case 'R':
            symbol_idx = 8;
            break;
        default:
            __builtin_unreachable();
    }
    return goto_table[state_id][symbol_idx];
}

StringBuilder *string_builder_init() {
    StringBuilder *sb = malloc(sizeof(StringBuilder));
    sb->str = malloc(sizeof(char) * (16 + 1));
    sb->capacity = 16;
    sb->length = 0;
    return sb;
}

void string_builder_append(StringBuilder *sb, const char *text) {
    size_t text_length = strlen(text);
    size_t new_length =
        sb->length + text_length + 1;  // +1 for the null terminator
    if (new_length > sb->capacity) {
        // Resize the buffer
        sb->capacity = new_length * 2;  // Double the capacity
        sb->str = (char *)realloc(
            sb->str,
            sb->capacity * sizeof(char));  // FIXME: deal with the NULL case
    }
    // Append the text to the end of the current string
    strcpy(sb->str + sb->length, text);
    sb->length = new_length - 1;
}

void string_builder_append_fmt(StringBuilder *sb, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the length of the formatted string
    size_t formattedLength = vsnprintf(NULL, 0, format, args);
    va_end(args);

    size_t new_length =
        sb->length + formattedLength + 1;  // +1 for the null terminator

    if (new_length > sb->capacity) {
        // Resize the buffer
        sb->capacity = new_length * 2;  // Double the capacity
        sb->str = (char *)realloc(sb->str, sb->capacity * sizeof(char));
    }

    va_start(args, format);

    // Append the formatted string to the end of the current string
    vsprintf(sb->str + sb->length, format, args);

    va_end(args);

    // Update the length
    sb->length = new_length - 1;
}

char *string_builder_build(StringBuilder *sb) {
    char *str = sb->str;
    free(sb);
    return str;
}

SLRTrace *slr_trace_init() {
    SLRTrace *trace = malloc(sizeof(SLRTrace));
    cc_deque_new(&trace->op_trace);
    cc_deque_new(&trace->stack_trace);
    return trace;
}

void destroy_slr_trace(SLRTrace *trace) {
    cc_deque_destroy_cb(trace->op_trace, free);
    cc_deque_destroy_cb(trace->stack_trace, free);
    free(trace);
}

char *stringify_slr_stack(SLRParser *parser) {
    StringBuilder *sb = string_builder_init();
    SLRItem *el = NULL;
    size_t deque_sz = cc_deque_size(parser->stack);
    for (size_t i = 0; i < deque_sz; i++) {
        cc_deque_get_at(parser->stack, i, (void *)&el);
        assert(el != NULL);
        stringify_slr_item(el, sb);
        if (i != deque_sz - 1) {
            string_builder_append(sb, ", ");
        }
    }
    return string_builder_build(sb);
}

char *stringify_token_ty(TokenTy ty) {
    char *symbol = NULL;
    switch (ty) {
        case Literal:
            symbol = "lit";
            break;
        case BoolDecl:
            symbol = "bool";
            break;
        case NatDecl:
            symbol = "nat";
            break;
        case FuncDecl:
            symbol = "fun";
            break;
        case QuestionMark:
            symbol = "?";
            break;
        case Colon:
            symbol = ":";
            break;
        case Semicolon:
            symbol = ";";
            break;
        case LeftParen:
            symbol = "(";
            break;
        case RightParen:
            symbol = ")";
            break;
        case Plus:
            symbol = "+";
            break;
        case Ampersand:
            symbol = "&";
            break;
        case Arrow:
            symbol = "->";
            break;
        case Less:
            symbol = "<";
            break;
        case Equal:
            symbol = "=";
            break;
        case Comment:
            __builtin_unreachable();
            break;
        case Identifier:
            symbol = "id";
            break;
        case Eof:
            symbol = "$";
            break;
        case Invalid:
            __builtin_unreachable();
            break;
    }
    return symbol;
}

void stringify_rule(Production *prod, StringBuilder *sb) {
    string_builder_append_fmt(sb, "%c", prod->lhs);
    string_builder_append(sb, " ->");
    for (size_t i = 0; i < prod->n_rhs; i++) {
        string_builder_append(sb, " ");
        switch (prod->rhs[i].ty) {
            case TERM_NON_TERMINAL:
                string_builder_append_fmt(sb, "%c", prod->rhs[i].value.nt);
                break;
            case TERM_TERMINAL:
                string_builder_append_fmt(
                    sb, "%s", stringify_token_ty(prod->rhs[i].value.t));
                break;
        }
    }
}

char *stringify_slr_op(SLRop *action, Grammar *grammar) {
    StringBuilder *sb = string_builder_init();
    switch (action->ty) {
        case SLR_SHIFT:
            string_builder_append_fmt(sb, "shift %d", action->value);
            break;
        case SLR_REDUCE:
            string_builder_append(sb, "reduce ");
            stringify_rule(&grammar->prods[action->value], sb);
            break;
        case SLR_GOTO:
        case SLR_EMPTY:
            __builtin_unreachable();
            break;
    }
    return string_builder_build(sb);
}

void stringify_slr_item(SLRItem *item, StringBuilder *sb) {
    if (item->ty == SLR_SYMBOL_TOKEN && item->symbol->token->ty == Eof) {
        string_builder_append(sb, "$");
        return;
    }

    if (item->ty == SLR_SYMBOL_VOID) {
        string_builder_append(sb, "0");
        return;
    }

    string_builder_append(sb, "(");
    if (item->ty == SLR_SYMBOL_TOKEN) {
        string_builder_append(sb, stringify_token_ty(item->symbol->token->ty));
    } else if (item->ty == SLR_SYMBOL_NON_TERMINAL) {
        string_builder_append_fmt(sb, "%c", item->symbol->nt);
    } else {
        return;
    }
    string_builder_append(sb, ", ");
    string_builder_append_fmt(sb, "%d", item->value);
    string_builder_append(sb, ")");
}

void slr_parser_display_trace(SLRParser *parser, FILE *fp) {
    assert(cc_deque_size(parser->trace->stack_trace) ==
           cc_deque_size(parser->trace->op_trace));
    char *op_trace_buf = NULL;
    char *stack_trace_buf = NULL;

    unsigned stack_trace_max_len = 0;
    for (size_t i = 0; i < cc_deque_size(parser->trace->stack_trace); i++) {
        cc_deque_get_at(parser->trace->stack_trace, i,
                        (void *)&stack_trace_buf);
        stack_trace_max_len = stack_trace_max_len > strlen(stack_trace_buf)
                                  ? stack_trace_max_len
                                  : strlen(stack_trace_buf);
    }

    unsigned digit_len = log10u(cc_deque_size(parser->trace->stack_trace));
    for (size_t i = 0; i < cc_deque_size(parser->trace->stack_trace); i++) {
        cc_deque_get_at(parser->trace->stack_trace, i,
                        (void *)&stack_trace_buf);
        cc_deque_get_at(parser->trace->op_trace, i, (void *)&op_trace_buf);
        printf("Step %-*lu:<%*s\t%s\n", digit_len, i, -stack_trace_max_len,
               stack_trace_buf, op_trace_buf);
        if (fp != NULL) {
            fprintf(fp, "Step %-*lu:<%*s\t%s\n", digit_len, i,
                    -stack_trace_max_len, stack_trace_buf, op_trace_buf);
        }
    }
}

/**
 * Take the ownership of the parse tree from the parser
 * @param parser
 * @return parser tree (with ownership)
 */
ParseTree *slr_parser_parse_tree(SLRParser *parser) {
    ParseTreeNode *root = parser->parse_tree;
    parser->parse_tree = NULL;
    return parse_tree_init(root);
}
