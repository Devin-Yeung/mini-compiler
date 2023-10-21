#include <assert.h>
#include "../lexer.h"

int main() {
    DFA dfa = {
            .n_accepts = N_ACCEPTS,
            .accepts = ACCEPTS,
            .table = TABLE,
            .start = 1,
            .state = 1,
    };

    // should pass
    assert(dfa_matches(&dfa, "abc"));
    assert(dfa_matches(&dfa, "0"));

    // should fail
    assert(!dfa_matches(&dfa, "00"));
    assert(!dfa_matches(&dfa, "0 "));
    return 0;
}