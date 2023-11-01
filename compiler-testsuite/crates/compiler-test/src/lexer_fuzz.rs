use crate::lexer_fuzz::FuzzyTokenTy::{BoolLit, Decl, Ident, NatLit, Punctuation, Whitespace};
use rand;
use rand::Rng;
use regex_generate::{Generator, DEFAULT_MAX_REPEAT};

#[derive(Clone, Copy, Eq, PartialEq)]
enum FuzzyTokenTy {
    Ident,
    Whitespace,
    Punctuation,
    BoolLit,
    NatLit,
    Decl,
}

pub fn rand_tokens(n: usize) -> Vec<String> {
    let mut rng = rand::thread_rng();
    let mut tokens = vec![];
    let mut prev = rand_token_ty();
    let mut count = n;

    while count > 0 {
        match prev {
            Ident | BoolLit | NatLit | Decl => match rng.gen_bool(0.5) {
                true => tokens.push(whitespace()),
                false => {
                    tokens.push(punctuation());
                    count -= 1;
                    if count <= 0 {
                        break;
                    }
                }
            },
            Whitespace | Punctuation => { /* Do not need a explicit whitespace */ }
        }
        prev = rand_token_ty();
        tokens.push(rand_token(prev));
        if prev != Whitespace {
            count -= 1;
        }
    }
    tokens
}

fn rand_token(ty: FuzzyTokenTy) -> String {
    match ty {
        Ident => ident(),
        Whitespace => whitespace(),
        Punctuation => punctuation(),
        BoolLit => bool_lit(),
        NatLit => nat_lit(),
        Decl => declaration(),
    }
}

fn rand_token_ty() -> FuzzyTokenTy {
    let ty = vec![Ident, Whitespace, Punctuation, BoolLit, NatLit, Decl];
    let mut rng = rand::thread_rng();
    let idx = rng.gen_range(0..ty.len());
    ty[idx]
}

fn ident() -> String {
    random_pattern(r"[a-zA-Z][a-zA-Z0-9]*")
}

fn whitespace() -> String {
    let mut gen = Generator::new(r"[\r\n\t\f\v ]+", rand::thread_rng(), 5).unwrap();
    let mut buffer = vec![];
    gen.generate(&mut buffer).unwrap();
    String::from_utf8(buffer).unwrap()
}

fn punctuation() -> String {
    random_pattern(r"[\?:;()+&<=]|->")
}

fn bool_lit() -> String {
    random_pattern(r"T|F")
}

fn nat_lit() -> String {
    random_pattern(r"0|[1-9][0-9]*")
}

fn declaration() -> String {
    random_pattern(r"bool|nat|fun")
}

fn random_pattern<P: AsRef<str>>(pattern: P) -> String {
    let mut gen = Generator::new(pattern.as_ref(), rand::thread_rng(), DEFAULT_MAX_REPEAT).unwrap();
    let mut buffer = vec![];
    gen.generate(&mut buffer).unwrap();
    let output = String::from_utf8(buffer).unwrap();
    return output;
}

#[cfg(test)]
mod tests {
    use crate::lexer_fuzz::rand_tokens;
    use mini_compiler_rs::lexer::Lexer;

    #[test]
    fn fuzzy_tokens() {
        let src_tokens = rand_tokens(100000);
        let src = src_tokens.join("");

        let src_tokens = src_tokens
            .into_iter()
            .filter(|tok| !tok.chars().all(char::is_whitespace))
            .collect::<Vec<_>>();

        let token = Lexer::new(&src).collect::<Vec<_>>();

        let actual_tokens = token
            .iter()
            .map(|tok| tok.lexeme.clone())
            .collect::<Vec<_>>();
        assert_eq!(src_tokens.len(), actual_tokens.len())
    }
}
