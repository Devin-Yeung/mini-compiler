#include "lexer.h"
#include <stdbool.h>

void dfa_reset(DFA *dfa) {
    dfa->state = dfa->start;
};

void dfa_next(DFA *dfa, char c) {
    dfa->state = dfa->table[(int) c][dfa->state];
};

bool dfa_is_accept(const DFA *dfa) {
    bool accept = false;
    for (unsigned i = 0; i < dfa->n_accepts; i++) {
        accept |= (dfa->state == dfa->accepts[i]);
    }
    return accept;
};

bool dfa_matches(DFA *dfa, const char *s) {
    unsigned cursor = 0;
    while (s[cursor] != '\0') {
        dfa_next(dfa, s[cursor++]);
    }

    bool accept = dfa_is_accept(dfa);
    dfa_reset(dfa);
    return accept;
};