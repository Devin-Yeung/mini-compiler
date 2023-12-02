#include "parser.h"

#include <stdio.h>

#include "deque.h"

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

    return parser;
}

void destroy_slr_item(SLRItem *item) {
    free(item->symbol);
    free(item);
}

void destroy_slr_item_cb(void *item) { destroy_slr_item((SLRItem *)item); }

void destroy_slr_parser(SLRParser *parser) {
    cc_deque_destroy_cb(parser->stack, destroy_slr_item_cb);
    free(parser);
}

ParserState slr_parser_step(SLRParser *parser, Token *tok) {
    printf("Step(%s)\n", tok->lexeme);
    SLRItem *last = NULL;
    cc_deque_get_last(parser->stack, (void *)&last);
    SLRop next = shift_reduce_table_get(parser->table->shift_reduce_table,
                                        last->value, tok->ty);
    if (next.ty == SLR_SHIFT) {
        printf("(Shift, %d)\n", next.value);
        SLRItem *item = slr_item_token(tok, SLR_SYMBOL_TOKEN, next.value);
        cc_deque_add_last(parser->stack, (void *)item);
    } else if (next.ty == SLR_REDUCE) {
        printf("(Reduce, %d)\n", next.value);
        if (next.value == 0) {
            printf("Accept!\n");
            return PARSER_ACCEPT;
        }
        Production prod = parser->grammar->prods[next.value];
        // Consume all the rhs item
        for (int i = (int)(prod.n_rhs - 1); i >= 0; i--) {
            if (cc_deque_remove_last(parser->stack, (void *)&last) == CC_OK) {
                // check if production rule is matched
                if (last->ty == SLR_SYMBOL_TOKEN) {
                    if (prod.rhs[i].ty == TERM_TERMINAL &&
                        prod.rhs[i].value.t == last->symbol->token->ty) {
                        continue;
                    } else {
                        return PARSER_REJECT;
                    }
                }

                if (last->ty == SLR_SYMBOL_NON_TERMINAL) {
                    if (prod.rhs[i].ty == TERM_NON_TERMINAL &&
                        prod.rhs[i].value.nt == last->symbol->nt) {
                        continue;
                    } else {
                        return PARSER_REJECT;
                    }
                }
                // TODO: reserve for building parse tree
            } else {
                // Do not have enough item to reduce a production rule
                return PARSER_REJECT;
            }
        }
        // Push the lhs item to the stack
        cc_deque_get_last(parser->stack, (void *)&last);
        SLRop op = goto_table_get(parser->table->goto_table, last->value,
                                  prod.lhs.value.nt);
        SLRItem *item =
            slr_item_nt(prod.lhs.value.nt, SLR_SYMBOL_NON_TERMINAL, op.value);
        printf("(GOTO, %d)\n", op.value);
        cc_deque_add_last(parser->stack, (void *)item);
        // deal with the incoming tok
        slr_parser_step(parser, tok);
    } else /* Empty Cell, Reject */ {
        printf("Reject due to the empty cell\n");
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
        case NatLit:
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
    printf("Checking Shift-Reduce Table [%d, %d]\n", state_id, symbol_idx);
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