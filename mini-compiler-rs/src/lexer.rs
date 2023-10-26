use std::ffi::CString;

use crate::gen::{self, destroy_lexer, lexer_next_token};
use crate::token::Token;

pub struct Lexer {
    inner: *mut gen::Lexer,
    _src: CString,
}

impl Lexer {
    pub fn new(src: &str) -> Lexer {
        let c_string = CString::new(src).unwrap();
        let inner = unsafe { gen::lexer_new(c_string.as_ptr()) };

        Lexer {
            inner,
            _src: c_string,
        }
    }

    pub fn next_token(&mut self) -> Token {
        let token: Token = unsafe { lexer_next_token(self.inner).into() };
        token
    }
}

impl Drop for Lexer {
    fn drop(&mut self) {
        unsafe {
            destroy_lexer(self.inner);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Lexer;

    #[test]
    fn it_works() {
        let mut lexer = Lexer::new("abc");
        println!("{:?}", lexer.next_token());
        println!("{:?}", lexer.next_token());
        println!("{:?}", lexer.next_token());
        println!("{:?}", lexer.next_token());
    }
}
