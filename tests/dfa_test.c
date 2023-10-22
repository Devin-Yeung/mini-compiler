#include <assert.h>
#include "../lexer.h"

int main() {
    DFA* dfa = &LEXER_DFA;

    // should pass
    assert(dfa_matches(dfa, "abc"));
    assert(dfa_matches(dfa, "0"));

    // should fail
    assert(!dfa_matches(dfa, "00"));
    assert(!dfa_matches(dfa, "0 "));
    return 0;
}