#[cfg(test)]
mod tests {
    use mini_compiler_rs::lexer::Lexer;
    use testsuite::unittest;

    unittest!(example_1, |_, src| {
        let token = Lexer::new(src).collect::<Vec<_>>();
        insta::assert_debug_snapshot!(token);
    });
}
