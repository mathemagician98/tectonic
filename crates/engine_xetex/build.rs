// Copyright 2016-2021 the Tectonic Project
// Licensed under the MIT License.

//! The Tectonic build script. Not only do we have internal C/C++ code, we
//! also depend on several external C/C++ libraries, so there's a lot to do
//! here. It would be great to streamline things.

use std::env;
use std::path::PathBuf;

fn main() {
    let bc_include_path = env::var("DEP_TECTONIC_BRIDGE_CORE_INCLUDE").unwrap();
    let icu_include_path = env::var("DEP_ICUUC_INCLUDE_PATH").unwrap();
    let xetex_layout_include_path = env::var("DEP_TECTONIC_XETEX_LAYOUT_INCLUDE_PATH").unwrap();
    let engine_teckit_include_path = env::var("DEP_TECTONIC_ENGINE_TECKIT_INCLUDE_PATH").unwrap();
    let engine_xdvipdfmx_include_path = env::var("DEP_TECTONIC_ENGINE_XDVIPDFMX_INCLUDE_PATH").unwrap();

    let out_dir: PathBuf = env::var("OUT_DIR").unwrap().into();
    let manifest_dir: PathBuf = env::var("CARGO_MANIFEST_DIR").unwrap().into();

    // Generate bindings for the C/C++ code to interface with backend Rust code.
    let mut cbindgen_header_path: PathBuf = out_dir.clone().into();
    cbindgen_header_path.push("xetex_bindings.h");

    println!("cargo:rerun-if-changed=src/lib.rs");

    let mut config = cbindgen::Config {
        cpp_compat: true,
        ..Default::default()
    };

    cbindgen::Builder::new()
        .with_config(config)
        .with_crate(&manifest_dir)
        .with_language(cbindgen::Language::C)
        .with_include_guard("XETEX_BINDINGS_H")
        .with_style(cbindgen::Style::Type)
        .rename_item("CoreBridgeState", "ttbc_state_t")
        .rename_item("History", "tt_history_t")
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(&cbindgen_header_path);     

    let mut c_build = cc::Build::new();
    let mut cxx_build = cc::Build::new();
    
    cxx_build.cpp(true);

    c_build.warnings(true);

    for flag in C_FLAGS {
        c_build.flag_if_supported(flag);
    }

    c_build
        .include(&bc_include_path)
        .include(&icu_include_path)
        .include(&engine_xdvipdfmx_include_path);

    for item in xetex_layout_include_path.split(';') {
        c_build.include(item);
    }
    
    for item in engine_teckit_include_path.split(';') {
        c_build.include(item);
    }

    for file in C_FILES {
        c_build.file(file);
    }
    
    c_build.compile("libtectonic_engine_xetex.a");
}

const C_FLAGS: &[&str] = &[
    "-Wall",
    "-Wcast-qual",
    "-Wdate-time",
    "-Wendif-labels",
    "-Wextra",
    "-Wextra-semi",
    "-Wformat=2",
    "-Winit-self",
    "-Wlogical-op",
    "-Wmissing-declarations",
    "-Wmissing-include-dirs",
    "-Wmissing-prototypes",
    "-Wmissing-variable-declarations",
    "-Wnested-externs",
    "-Wold-style-definition",
    "-Wpointer-arith",
    "-Wredundant-decls",
    "-Wstrict-prototypes",
    "-Wsuggest-attribute=format",
    "-Wswitch-bool",
    "-Wundef",
    "-Wwrite-strings",
    // TODO: Fix existing warnings before enabling these:
    // "-Wbad-function-cast",
    // "-Wcast-align",
    // "-Wconversion",
    // "-Wdouble-promotion",
    // "-Wshadow",
    // "-Wsuggest-attribute=const",
    // "-Wsuggest-attribute=noreturn",
    // "-Wsuggest-attribute=pure",
    // "-Wunreachable-code-aggresive",
    "-Wno-unused-parameter",
    "-Wno-implicit-fallthrough",
    "-Wno-sign-compare",
    "-std=gnu11",
];

const C_FILES: &[&str] = &[
    "xetex/engine-interface.c",
    "xetex/errors.c",
    "xetex/ext.c",
    "xetex/io.c",
    "xetex/linebreak.c",
    "xetex/math.c",
    "xetex/output.c",
    "xetex/pagebuilder.c",
    "xetex/pic.c",
    "xetex/scaledmath.c",
    "xetex/shipout.c",
    "xetex/stringpool.c",
    "xetex/synctex.c",
    "xetex/texmfmp.c",
    "xetex/xetex.c",
];

const CXX_FLAGS: &[&str] = &[
    "-std=c++14",
    "-Wall",
    "-Wdate-time",
    "-Wendif-labels",
    "-Wextra",
    "-Wformat=2",
    "-Wlogical-op",
    "-Wmissing-declarations",
    "-Wmissing-include-dirs",
    "-Wpointer-arith",
    "-Wredundant-decls",
    "-Wsuggest-attribute=noreturn",
    "-Wsuggest-attribute=format",
    "-Wshadow",
    "-Wswitch-bool",
    "-Wundef",
    // TODO: Fix existing warnings before enabling these:
    // "-Wdouble-promotion",
    // "-Wcast-align",
    // "-Wconversion",
    // "-Wmissing-variable-declarations",
    "-Wextra-semi",
    // "-Wsuggest-attribute=const",
    // "-Wsuggest-attribute=pure",
    // "-Wunreachable-code-aggresive",
    "-Wno-unused-parameter",
    "-Wno-implicit-fallthrough",
    "-fno-exceptions",
    "-fno-rtti",
];

const CXX_FILES: &[&str] = &[
    "xetex/XeTeXOTMath.cpp",
];
