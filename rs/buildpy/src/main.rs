#![allow(dead_code)]
mod config;
mod core;
mod ops;
use ops::log as log;

use clap::Parser;
use std::env;
// use std::path::PathBuf;

// extern crate simplelog;
//pub use simplelog::{info,debug,warn,error};

/// Builds python from source
#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Cli {

    /// Optional name to operate on
    // name: Option<String>,

    /// Python version
    #[arg(short, long, default_value = "3.11.8")]
    pyversion: Option<String>,

    /// Config name
    #[arg(short, long, default_value = "static_max")]
    config: Option<String>,

    /// Config options
    #[arg(short, long)]
    opts: Option<Vec<String>>,


    // version, _ := cmd.Flags().GetString("version")
    // config, _ := cmd.Flags().GetString("config")
    // pkgs, _ := cmd.Flags().GetStringSlice("pkgs")
    // opts, _ := cmd.Flags().GetStringSlice("opts")
    // jobs, _ := cmd.Flags().GetInt("jobs")
    // optimize, _ := cmd.Flags().GetBool("optimize")
    // reset, _ := cmd.Flags().GetBool("reset")
    // debug, _ := cmd.Flags().GetBool("debug")
    // git, _ := cmd.Flags().GetBool("git")

}

fn demo() {
    let cfg = config::Config::new("static_max".to_string(), "3.12.2".to_string());

    let _proj = core::Project::new();
    println!("project cwd: {:?}", _proj.cwd);

    let mut _builder = core::Builder::new();
    _builder.process();

    let _serialized = serde_yaml::to_string(&cfg).unwrap();

    ops::run("python3", &["-c", "print('ok')"]);

    // println!("{serialized}");

    // // println!("cfg is {:?}", cfg);
    // for key in cfg.exts.keys() {
    //     println!("{key}");
    // }
    // // cfg.static_to_disabled(vecs!["_decimal"]);
    // println!("bye..");

    ops::cmd("python3", vec!["--version"], ".");


    println!("{}", env::consts::OS); // Prints the current OS.

    log::debug!("This level is currently not enabled for any logger");
    log::info!("This only appears in the log file");
    log::warn!("This is a warning");
    log::error!("Bright red error");
}

fn main() {
    log::init_logging();

    let args = Cli::parse();

    // if let Some(name) = args.name.as_deref() {
    //     log::info!("Value for name: {name}");
    // }

    if let Some(ver) = args.pyversion.as_deref() {
        log::info!("Value for pyversion: {ver}");        
    }

    if let Some(cfg) = args.config.as_deref() {
        log::info!("Value for config: {cfg}");        
    }

    if let Some(opts) = args.opts.as_deref() {
        log::info!("Value for opts: {opts:?}");        
    }
}