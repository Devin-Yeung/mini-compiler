use std::ffi::CStr;

use crate::gen;

#[derive(Debug)]
pub struct Token {
    pub lexeme: String,
    pub ty: TokenTy,
    pub span: Span,
}

impl From<*mut gen::Token> for Token {
    fn from(value: *mut gen::Token) -> Self {
        let token = unsafe { *value };
        let lexeme = unsafe { CStr::from_ptr(token.lexeme) }
            .to_string_lossy()
            .to_string();
        let ret = Token {
            lexeme,
            ty: token.ty.into(),
            span: token.span.into(),
        };
        unsafe { gen::destroy_token(value) };
        ret
    }
}

#[derive(Debug)]
pub struct Span {
    pub start: usize,
    pub end: usize,
}

impl From<*mut gen::Span> for Span {
    fn from(value: *mut gen::Span) -> Self {
        if value.is_null() {
            return Span { start: 0, end: 0 };
        }
        let inner = unsafe { *value };
        let span = Span {
            start: inner.start as usize,
            end: inner.end as usize,
        };
        span
    }
}

#[derive(Debug, Eq, PartialEq)]
pub enum TokenTy {
    Literal,      /* Literal */
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
    Invalid,      /* Invalid token */
}

impl From<gen::TokenTy> for TokenTy {
    fn from(value: gen::TokenTy) -> Self {
        match value {
            gen::TokenTy_Literal => TokenTy::Literal,
            gen::TokenTy_BoolDecl => TokenTy::BoolDecl,
            gen::TokenTy_NatDecl => TokenTy::NatDecl,
            gen::TokenTy_FuncDecl => TokenTy::FuncDecl,
            gen::TokenTy_QuestionMark => TokenTy::QuestionMark,
            gen::TokenTy_Colon => TokenTy::Colon,
            gen::TokenTy_Semicolon => TokenTy::Semicolon,
            gen::TokenTy_LeftParen => TokenTy::LeftParen,
            gen::TokenTy_RightParen => TokenTy::RightParen,
            gen::TokenTy_Plus => TokenTy::Plus,
            gen::TokenTy_Ampersand => TokenTy::Ampersand,
            gen::TokenTy_Arrow => TokenTy::Arrow,
            gen::TokenTy_Less => TokenTy::Less,
            gen::TokenTy_Equal => TokenTy::Equal,
            gen::TokenTy_Comment => TokenTy::Comment,
            gen::TokenTy_Identifier => TokenTy::Identifier,
            gen::TokenTy_Eof => TokenTy::Eof,
            gen::TokenTy_Invalid => TokenTy::Invalid,
            _ => unreachable!(),
        }
    }
}
