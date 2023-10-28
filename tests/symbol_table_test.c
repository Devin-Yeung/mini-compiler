#include "../symbol_table.h"

#include <assert.h>
#include <stdio.h>

SymbolTy gen_ty(unsigned i) {
    switch (i % 3) {
        case 0:
            return FuncTy;
        case 1:
            return BoolTy;
        case 2:
            return NatTy;
        default:
            assert(0);
    }
}

void fuzzy_test() {
    SymbolTable *t = symbol_table_new();
    char buf[64];
    for (unsigned i = 0; i < 1000; i++) {
        snprintf(buf, sizeof(buf), "%u", i);
        symbol_table_insert(t, buf, gen_ty(i));
    }
    for (unsigned i = 0; i < 1000; i++) {
        snprintf(buf, sizeof(buf), "%u", i);
        assert(symbol_table_find(t, buf) == gen_ty(i));
    }
    symbol_table_walk(t);
    symbol_table_destroy(t);
}

void duplicate_symbol_test() {
    SymbolTable *t = symbol_table_new();
    symbol_table_insert(t, "1", FuncTy);
    symbol_table_insert(t, "1", BoolTy);
    assert(symbol_table_find(t, "1") == FuncTy);
    symbol_table_destroy(t);
}

int main() {
    fuzzy_test();
    duplicate_symbol_test();
}