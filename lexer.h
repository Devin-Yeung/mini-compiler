#ifndef MINI_COMPILER_LEXER_H
#define MINI_COMPILER_LEXER_H

#include <stdbool.h>

typedef struct Span {
    unsigned start;
    unsigned end;
} Span;

Span *span_new(unsigned start, unsigned end);

typedef enum TokenTy {
    BoolLit,      /* Boolean Literal */
    NatLit,       /* Natural Number Literal */
    BoolDecl,     /* The variable type Boolean */
    NatDecl,      /* The variable type Natural */
    FuncDecl,     /* The variable type Function */
    QuestionMark, /* ? */
    Colon,        /* : */
    Semicolon,    /* ; */
    LeftParen,    /* ( */
    RightParen,   /* ) */
    Plus,         /* + */
    Ampersand,    /* & */
    Arrow,        /* -> */
    Less,         /* < */
    Equal,        /* = */
    Comment,      /* [] */
    Identifier,   /* Identifier */
    Eof,          /* End of file */
} TokenTy;

typedef struct Token {
    char *lexeme;
    Span *span;
    TokenTy ty;
} Token;

Token *eof_token();

typedef struct DFA {
    const int (*table)[9];    /* TODO: better Solution? */
    const unsigned start;     /* start state */
    const unsigned n_accepts; /* number of accept states */
    const unsigned *accepts;  /* accept states */
    unsigned state;           /* current state */
} DFA;

void dfa_reset(DFA *dfa);
void dfa_next(DFA *dfa, char c);
bool dfa_matches(DFA *dfa, const char *s);

typedef struct Lexer {
    DFA *dfa;
    unsigned start; /* start position of the current lexeme */
    unsigned end;   /* end position of the current lexeme */
    char *src;
    unsigned len; /* len of the source code, null byte EXCLUDED */
} Lexer;

Lexer *lexer_new(char *src);
Token *lexer_next_token(Lexer *lexer);

static const unsigned ACCEPTS[5] = {2, 4, 5, 7, 8};
static const unsigned N_ACCEPTS = sizeof(ACCEPTS) / sizeof(ACCEPTS[0]);

static const int TABLE[256][9] = {
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 8, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 8, 0, 0, 0, 0, 6, 0, 0}, {0, 8, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 8, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 3, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 8, 2, 0, 0, 5, 6, 0, 0}, {0, 2, 2, 0, 0, 5, 6, 0, 0},
    {0, 2, 2, 0, 0, 5, 6, 0, 0}, {0, 2, 2, 0, 0, 5, 6, 0, 0},
    {0, 2, 2, 0, 0, 5, 6, 0, 0}, {0, 2, 2, 0, 0, 5, 6, 0, 0},
    {0, 2, 2, 0, 0, 5, 6, 0, 0}, {0, 2, 2, 0, 0, 5, 6, 0, 0},
    {0, 2, 2, 0, 0, 5, 6, 0, 0}, {0, 2, 2, 0, 0, 5, 6, 0, 0},
    {0, 8, 0, 0, 0, 0, 6, 0, 0}, {0, 8, 0, 0, 0, 0, 6, 0, 0},
    {0, 8, 0, 0, 0, 0, 6, 0, 0}, {0, 8, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 4, 0, 0, 6, 0, 0}, {0, 8, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 6, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 7, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 5, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 5, 0, 0, 0, 5, 6, 0, 0},
    {0, 5, 0, 0, 0, 5, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0},
    {0, 1, 0, 0, 0, 0, 6, 0, 0}, {0, 1, 0, 0, 0, 0, 6, 0, 0}};

static struct DFA LEXER_DFA = {
    .n_accepts = N_ACCEPTS,
    .accepts = ACCEPTS,
    .table = TABLE,
    .start = 1,
    .state = 1,
};

#endif  // MINI_COMPILER_LEXER_H
