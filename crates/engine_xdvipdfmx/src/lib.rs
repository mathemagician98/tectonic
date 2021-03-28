use std::time::SystemTime;
use std::ffi::CStr;
use std::ffi::CString;
use tectonic_bridge_core::{CoreBridgeLauncher, EngineAbortedError, IoEventBackend};
use tectonic_status_base::StatusBackend;
use tectonic_io_base::stack::IoStack;
use tectonic_errors::Result;

#[repr(C)]
pub struct XdvipdfmxConfig {
    paperspec: *const libc::c_char,
}

pub struct XdvipdfmxEngine {
    enable_compression: bool,
    deterministic_tags: bool,
    build_date: SystemTime,
}

impl XdvipdfmxEngine {
    pub fn new() -> XdvipdfmxEngine {
        XdvipdfmxEngine {
            enable_compression: true,
            deterministic_tags: false,
            build_date: SystemTime::UNIX_EPOCH,
        }
    }

    pub fn with_compression(mut self, enable_compression: bool) -> Self {
        self.enable_compression = enable_compression;
        self
    }

    pub fn with_deterministic_tags(mut self, flag: bool) -> Self {
        self.deterministic_tags = flag;
        self
    }

    /// Sets the date and time used by the xdvipdfmx engine. This value is used
    /// as a source of entropy and is written to the output PDF. When expecting
    /// reproducible builds, this should be set to a static value, like its
    /// default value UNIX_EPOCH.
    pub fn with_date(mut self, date: SystemTime) -> Self {
        self.build_date = date;
        self
    }

    pub fn process(
        &mut self,
        io: &mut IoStack,
        events: &mut dyn IoEventBackend,
        status: &mut dyn StatusBackend,
        dvi: &str,
        pdf: &str
    ) -> Result<i32> {
        // We default to "letter" paper size by default
        let paperspec = CStr::from_bytes_with_nul(b"letter\0").unwrap();

        let config = XdvipdfmxConfig {
            paperspec: paperspec.as_ptr(),
        };

        let cdvi = CString::new(dvi)?;
        let cpdf = CString::new(pdf)?;

        let mut launcher = CoreBridgeLauncher::new(io, events, status);

        launcher
            .with_global_lock(|state| {
                let r = unsafe {
                    c_api::tt_engine_xdvipdfmx_main(
                        state,
                        &config,
                        cdvi.as_ptr(),
                        cpdf.as_ptr(),
                        self.enable_compression,
                        self.deterministic_tags,
                        self.build_date
                            .duration_since(SystemTime::UNIX_EPOCH)
                            .expect("invalid build date")
                            .as_secs() as libc::time_t,
                    )
                };

                match r {
                    99 => Err(EngineAbortedError::new_abort_indicator().into()),
                    x => Ok(x as i32),
                }
            })
            .map_err(|e| e.into())
    }
}

impl Default for XdvipdfmxEngine {
    fn default() -> Self {
        XdvipdfmxEngine::new()
    }
}

mod c_api {
    use tectonic_bridge_core::CoreBridgeState;
    use super::*;

    #[allow(improper_ctypes)]
    extern "C" {
        pub fn tt_engine_xdvipdfmx_main(
            api: &mut CoreBridgeState,
            config: *const XdvipdfmxConfig,
            dvi_name: *const libc::c_char,
            pdf_name: *const libc::c_char,
            compress: bool,
            deterministic_tags: bool,
            build_date: libc::time_t,
        ) -> libc::c_int;
    }
}