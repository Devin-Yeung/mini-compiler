use compiler_test::lexer_fuzz::rand_tokens;
use std::io::Write;

fn main() {
    // get args
    let args: Vec<String> = std::env::args().collect();
    let n = args[1].parse::<usize>().unwrap();
    let output = args[2].clone();

    // create file
    let mut file = std::fs::File::create(&output).unwrap();
    println!("Generate {n} tokens to {output} ...");
    let tokens = rand_tokens(n).join("");
    file.write_all(tokens.as_bytes()).unwrap();
}
