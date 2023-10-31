use bindgen;
use std::fs::canonicalize;
use std::path::PathBuf;
use std::{env, fs};

use bindgen::CargoCallbacks;

fn c_source_files() -> impl Iterator<Item = PathBuf> {
    let source_dir = PathBuf::from("../../../");
    let mut paths = Vec::new();
    for file in ["lexer.c"] {
        paths.push(canonicalize(source_dir.join(file)).unwrap());
    }
    paths.into_iter()
}

fn main() {
    // This is the directory where the `c` library is located.
    let libdir_path = PathBuf::from(env::var_os("OUT_DIR").unwrap()).join("build");
    let _ = fs::create_dir_all(&libdir_path);
    // Canonicalize the path as `rustc-link-search` requires an absolute path.
    let libdir_path = libdir_path
        .canonicalize()
        .expect("cannot canonicalize path");

    let mut cfg = cc::Build::new();
    for file in c_source_files() {
        cfg.file(file);
    }
    cfg.out_dir(&libdir_path).warnings(false); // suppress warning

    cfg.compile("compiler");

    // This is the path to the `c` headers file.
    let headers_path = PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("mini_compiler.h");
    let headers_path_str = headers_path.to_str().expect("Path is not a valid string");

    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search={}", libdir_path.to_str().unwrap());

    // Tell cargo to tell rustc to link our `hello` library. Cargo will
    // automatically know it must look for a `libhello.a` file.
    println!("cargo:rustc-link-lib=compiler");

    // Tell cargo to invalidate the built crate whenever the header changes.
    println!("cargo:rerun-if-changed={}", headers_path_str);
    // Tell cargo to invalidate the built crate whenever the c source file changes.
    for file in c_source_files() {
        println!("cargo:rerun-if-changed={}", file.to_str().unwrap());
    }

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header(headers_path_str)
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap()).join("bindings.rs");
    bindings
        .write_to_file(out_path)
        .expect("Couldn't write bindings!");
}
