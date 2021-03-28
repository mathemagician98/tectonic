use std::env;
use std::path::PathBuf;

fn main() {
    let out_dir: PathBuf = env::var("OUT_DIR").unwrap().into();
    let manifest_dir: PathBuf = env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let include_dir = manifest_dir.join("include");

    println!("cargo:include-path={}", include_dir.to_str().unwrap());

    let mut cbindgen_header_path: PathBuf = out_dir.clone().into();
    cbindgen_header_path.push("xdvipdfmx_bindings.h");

    let mut config = cbindgen::Config {
        cpp_compat: true,
        ..Default::default()
    };

    cbindgen::Builder::new()
        .with_config(config)
        .with_crate(&manifest_dir)
        .with_language(cbindgen::Language::C)
        .with_include_guard("XDVIPDFMX_BINDINGS_H")
        .with_style(cbindgen::Style::Type)
        .rename_item("CoreBridgeState", "ttbc_state_t")
        .rename_item("XdvipdfmxConfig", "tt_xdvipdfmx_config_t")
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(&cbindgen_header_path);
    
    let build = cc::Build::new();
    build.compile("libtectonic_engine_xdvipdfmx.a");
}

static C_FILES = &[
    "xdvipdfmx/"
]