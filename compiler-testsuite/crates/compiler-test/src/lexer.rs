use std::fs;
use std::path::PathBuf;

macro_rules! path {
    ($root:expr, $($segment:expr),+) => {{
        let mut root: PathBuf = $root;
        $(root.push($segment);)*
        root
    }}
}

fn snap<S: AsRef<str>>(src_name: S) {
    let root = PathBuf::from(std::env!("CARGO_MANIFEST_DIR"));
    let source_path = path!(
        root.clone(),
        "snapshots",
        "lexer",
        "input",
        src_name.as_ref()
    );
    let snapshot_path = path!(root.clone(), "snapshots", "lexer", "output");

    insta::with_settings!({
        snapshot_path => snapshot_path,
        prepend_module_to_snapshot => false,
        snapshot_suffix => src_name.as_ref(),
    },{
        let source = fs::read_to_string(source_path).unwrap();
        let token = mini_compiler_rs::lexer::Lexer::new(&source).collect::<Vec<_>>();
        insta::assert_debug_snapshot!(token);
    });
}

#[cfg(test)]
mod tests {
    use crate::lexer::snap;
    use std::fs;
    use std::path::PathBuf;

    #[test]
    fn lexer_snapshots() {
        let src_dir = path!(
            PathBuf::from(std::env!("CARGO_MANIFEST_DIR")),
            "snapshots",
            "lexer",
            "input"
        );

        for entry in fs::read_dir(src_dir).unwrap() {
            let path = entry.unwrap().path();

            if path.is_file() {
                let path = path.file_name().unwrap().to_str().unwrap();
                snap(path);
            }
        }
    }
}
