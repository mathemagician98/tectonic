use std::env;
use std::path::PathBuf;
use cc;

fn main() {
    let manifest_path: PathBuf = env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let bc_include_path: PathBuf = env::var("DEP_TECTONIC_BRIDGE_CORE_INCLUDE").unwrap().into();
    let flate_include_path: PathBuf = env::var("DEP_TECTONIC_BRIDGE_FLATE_INCLUDE").unwrap().into();
    let include_path = manifest_path.join("include");

    println!("cargo:include-path={}", include_path.to_str().unwrap());

    let mut c_build = cc::Build::new();
    let mut cxx_build = cc::Build::new();

    cxx_build.cpp(true);

    for item in &[include_path, bc_include_path, flate_include_path] {
        c_build.include(item);
        cxx_build.include(item);
    }

    c_build.file("src/NormalizationData.c");

    cxx_build.file("src/Engine.cpp");

    c_build.compile("libtectonic_engine_teckit_c.a");
    cxx_build.compile("libtectonic_engine_teckit_cxx.a");
}