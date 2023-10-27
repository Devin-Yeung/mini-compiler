#include "../symbol_table.h"

#include <assert.h>

int main() {
    SymbolTable* t = symbol_table_new();
    symbol_table_insert(t, "hello", FuncTy);
    symbol_table_insert(t, "world", NatTy);
    symbol_table_insert(t, "!", BoolTy);
    assert(symbol_table_find(t, "hello") == FuncTy);
    assert(symbol_table_find(t, "world") == NatTy);
    assert(symbol_table_find(t, "!") == BoolTy);
    symbol_table_destroy(t);
}