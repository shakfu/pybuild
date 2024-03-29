use serde::{Deserialize, Serialize};
use std::collections::HashMap;

// #[derive(Serialize, Deserialize, Debug)]
// pub struct Person {
//     pub name: String,
//     pub age: i32,
// }

#[derive(Serialize, Debug)]
pub struct Config1<'a> {
    pub name: String,
    pub version: String,
    pub headers: Vec<&'a str>,
    pub exts: HashMap<&'a str, Vec<&'a str>>,
    pub core: Vec<&'a str>,
    pub statik: Vec<&'a str>,
    pub shared: Vec<&'a str>,
    pub disabled: Vec<&'a str>,
}

impl<'a> Config1<'a> {
    pub fn new(name: String, version: String) -> Self {
        Self {
            name: name.clone(),
            version: version.clone(),
            headers: vec![
                "DESTLIB=$(LIBDEST)",
                "MACHDESTLIB=$(BINLIBDEST)",
                "DESTPATH=",
                "SITEPATH=",
                "TESTPATH=",
                "COREPYTHONPATH=$(DESTPATH)$(SITEPATH)$(TESTPATH)",
                "PYTHONPATH=$(COREPYTHONPATH)",
                "OPENSSL=$(srcdir)/../../install/openssl",
                "BZIP2=$(srcdir)/../../install/bzip2",
                "LZMA=$(srcdir)/../../install/xz",
            ],
            exts: HashMap::from([
                ("name", vec!["name", "dest"]),
                ("_abc", vec!["_abc.c"]),
                ("_asyncio", vec!["_asynciomodule.c"]),
                ("_bisect", vec!["_bisectmodule.c"]),
                (
                    "_blake2",
                    vec![
                        "_blake2/blake2module.c",
                        "_blake2/blake2b_impl.c",
                        "_blake2/blake2s_impl.c",
                    ],
                ),
                (
                    "_bz2",
                    vec![
                        "_bz2module.c",
                        "-I$(BZIP2)/include",
                        "-L$(BZIP2)/lib",
                        "$(BZIP2)/lib/libbz2.a",
                    ],
                ),
                ("_codecs", vec!["_codecsmodule.c"]),
                ("_codecs_cn", vec!["cjkcodecs/_codecs_cn.c"]),
                ("_codecs_hk", vec!["cjkcodecs/_codecs_hk.c"]),
                ("_codecs_iso2022", vec!["cjkcodecs/_codecs_iso2022.c"]),
                ("_codecs_jp", vec!["cjkcodecs/_codecs_jp.c"]),
                ("_codecs_kr", vec!["cjkcodecs/_codecs_kr.c"]),
                ("_codecs_tw", vec!["cjkcodecs/_codecs_tw.c"]),
                ("_collections", vec!["_collectionsmodule.c"]),
                ("_contextvars", vec!["_contextvarsmodule.c"]),
                ("_csv", vec!["_csv.c"]),
                (
                    "_ctypes",
                    vec![
                        "_ctypes/_ctypes.c",
                        "_ctypes/callbacks.c",
                        "_ctypes/callproc.c",
                        "_ctypes/stgdict.c",
                        "_ctypes/cfield.c",
                        "-ldl",
                        "-lffi",
                        "-DHAVE_FFI_PREP_CIF_VAR",
                        "-DHAVE_FFI_PREP_CLOSURE_LOC",
                        "-DHAVE_FFI_CLOSURE_ALLOC",
                    ],
                ),
                (
                    "_curses",
                    vec!["-lncurses", "-lncursesw", "-ltermcap", "_cursesmodule.c"],
                ),
                (
                    "_curses_panel",
                    vec!["-lpanel", "-lncurses", "_curses_panel.c"],
                ),
                ("_datetime", vec!["_datetimemodule.c"]),
                (
                    "_dbm",
                    vec!["_dbmmodule.c", "-lgdbm_compat", "-DUSE_GDBM_COMPAT"],
                ),
                ("_decimal", vec!["_decimal/_decimal.c", "-DCONFIG_64=1"]),
                ("_elementtree", vec!["_elementtree.c"]),
                (
                    "_functools",
                    vec![
                        "-DPy_BUILD_CORE_BUILTIN",
                        "-I$(srcdir)/Include/internal",
                        "_functoolsmodule.c",
                    ],
                ),
                ("_gdbm", vec!["_gdbmmodule.c", "-lgdbm"]),
                (
                    "_hashlib",
                    vec![
                        "_hashopenssl.c",
                        "-I$(OPENSSL)/include",
                        "-L$(OPENSSL)/lib",
                        "$(OPENSSL)/lib/libcrypto.a",
                    ],
                ),
                ("_heapq", vec!["_heapqmodule.c"]),
                (
                    "_io",
                    vec![
                        "_io/_iomodule.c",
                        "_io/iobase.c",
                        "_io/fileio.c",
                        "_io/bytesio.c",
                        "_io/bufferedio.c",
                        "_io/textio.c",
                        "_io/stringio.c",
                    ],
                ),
                ("_json", vec!["_json.c"]),
                (
                    "_locale",
                    vec!["-DPy_BUILD_CORE_BUILTIN", "_localemodule.c"],
                ),
                ("_lsprof", vec!["_lsprof.c", "rotatingtree.c"]),
                (
                    "_lzma",
                    vec![
                        "_lzmamodule.c",
                        "-I$(LZMA)/include",
                        "-L$(LZMA)/lib",
                        "$(LZMA)/lib/liblzma.a",
                    ],
                ),
                ("_md5", vec!["md5module.c"]),
                ("_multibytecodec", vec!["cjkcodecs/multibytecodec.c"]),
                (
                    "_multiprocessing",
                    vec![
                        "_multiprocessing/multiprocessing.c",
                        "_multiprocessing/semaphore.c",
                    ],
                ),
                ("_opcode", vec!["_opcode.c"]),
                ("_operator", vec!["_operator.c"]),
                ("_pickle", vec!["_pickle.c"]),
                ("_posixshmem", vec!["_multiprocessing/posixshmem.c"]),
                ("_posixsubprocess", vec!["_posixsubprocess.c"]),
                ("_queue", vec!["_queuemodule.c"]),
                ("_random", vec!["_randommodule.c"]),
                ("_scproxy", vec!["_scproxy.c"]),
                ("_sha1", vec!["sha1module.c"]),
                ("_sha256", vec!["sha256module.c"]),
                ("_sha3", vec!["_sha3/sha3module.c"]),
                ("_sha512", vec!["sha512module.c"]),
                (
                    "_signal",
                    vec![
                        "-DPy_BUILD_CORE_BUILTIN",
                        "-I$(srcdir)/Include/internal",
                        "signalmodule.c",
                    ],
                ),
                ("_socket", vec!["socketmodule.c"]),
                (
                    "_sqlite3",
                    vec![
                        "_sqlite/blob.c",
                        "_sqlite/connection.c",
                        "_sqlite/cursor.c",
                        "_sqlite/microprotocols.c",
                        "_sqlite/module.c",
                        "_sqlite/prepare_protocol.c",
                        "_sqlite/row.c",
                        "_sqlite/statement.c",
                        "_sqlite/util.c",
                    ],
                ),
                ("_sre", vec!["_sre/sre.c", "-DPy_BUILD_CORE_BUILTIN"]),
                (
                    "_ssl",
                    vec![
                        "_ssl.c",
                        "-I$(OPENSSL)/include",
                        "-L$(OPENSSL)/lib",
                        "$(OPENSSL)/lib/libcrypto.a",
                        "$(OPENSSL)/lib/libssl.a",
                    ],
                ),
                ("_stat", vec!["_stat.c"]),
                ("_statistics", vec!["_statisticsmodule.c"]),
                ("_struct", vec!["_struct.c"]),
                ("_symtable", vec!["symtablemodule.c"]),
                (
                    "_thread",
                    vec![
                        "-DPy_BUILD_CORE_BUILTIN",
                        "-I$(srcdir)/Include/internal",
                        "_threadmodule.c",
                    ],
                ),
                ("_tracemalloc", vec!["_tracemalloc.c"]),
                ("_typing", vec!["_typingmodule.c"]),
                ("_uuid", vec!["_uuidmodule.c"]),
                ("_weakref", vec!["_weakref.c"]),
                ("_zoneinfo", vec!["_zoneinfo.c"]),
                ("array", vec!["arraymodule.c"]),
                ("atexit", vec!["atexitmodule.c"]),
                ("binascii", vec!["binascii.c"]),
                ("cmath", vec!["cmathmodule.c"]),
                ("errno", vec!["errnomodule.c"]),
                ("faulthandler", vec!["faulthandler.c"]),
                ("fcntl", vec!["fcntlmodule.c"]),
                ("grp", vec!["grpmodule.c"]),
                ("itertools", vec!["itertoolsmodule.c"]),
                ("math", vec!["mathmodule.c"]),
                ("mmap", vec!["mmapmodule.c"]),
                ("ossaudiodev", vec!["ossaudiodev.c"]),
                (
                    "posix",
                    vec![
                        "-DPy_BUILD_CORE_BUILTIN",
                        "-I$(srcdir)/Include/internal",
                        "posixmodule.c",
                    ],
                ),
                ("pwd", vec!["pwdmodule.c"]),
                (
                    "pyexpat",
                    vec![
                        "expat/xmlparse.c",
                        "expat/xmlrole.c",
                        "expat/xmltok.c",
                        "pyexpat.c",
                        "-I$(srcdir)/Modules/expat",
                        "-DHAVE_EXPAT_CONFIG_H",
                        "-DUSE_PYEXPAT_CAPI",
                        "-DXML_DEV_URANDOM",
                    ],
                ),
                ("readline", vec!["readline.c", "-lreadline", "-ltermcap"]),
                ("resource", vec!["resource.c"]),
                ("select", vec!["selectmodule.c"]),
                ("spwd", vec!["spwdmodule.c"]),
                ("syslog", vec!["syslogmodule.c"]),
                ("termios", vec!["termios.c"]),
                (
                    "time",
                    vec![
                        "-DPy_BUILD_CORE_BUILTIN",
                        "-I$(srcdir)/Include/internal",
                        "timemodule.c",
                    ],
                ),
                ("unicodedata", vec!["unicodedata.c"]),
                ("zlib", vec!["zlibmodule.c", "-lz"]),
            ]),
            core: vec![
                "_abc",
                "_codecs",
                "_collections",
                "_functools",
                "_io",
                "_locale",
                "_operator",
                "_signal",
                "_sre",
                "_stat",
                "_symtable",
                "_thread",
                "_tracemalloc",
                "_weakref",
                "atexit",
                "errno",
                "faulthandler",
                "itertools",
                "posix",
                "pwd",
                "time",
            ],
            statik: vec![
                "_asyncio",
                "_bisect",
                "_blake2",
                "_bz2",
                "_contextvars",
                "_csv",
                "_datetime",
                "_decimal",
                "_elementtree",
                "_hashlib",
                "_heapq",
                "_json",
                "_lsprof",
                "_lzma",
                "_md5",
                "_multibytecodec",
                "_multiprocessing",
                "_opcode",
                "_pickle",
                "_posixshmem",
                "_posixsubprocess",
                "_queue",
                "_random",
                "_sha1",
                "_sha256",
                "_sha3",
                "_sha512",
                "_socket",
                "_sqlite3",
                "_ssl",
                "_statistics",
                "_struct",
                "_typing",
                "_uuid",
                "_zoneinfo",
                "array",
                "binascii",
                "cmath",
                "fcntl",
                "grp",
                "math",
                "mmap",
                "pyexpat",
                "readline",
                "select",
                "unicodedata",
                "zlib",
            ],
            shared: vec![],
            disabled: vec![
                "_codecs_cn",
                "_codecs_hk",
                "_codecs_iso2022",
                "_codecs_jp",
                "_codecs_kr",
                "_codecs_tw",
                "_crypt",
                "_ctypes",
                "_curses",
                "_curses_panel",
                "_dbm",
                "_scproxy",
                "_tkinter",
                "_xxsubinterpreters",
                "audioop",
                "nis",
                "ossaudiodev",
                "resource",
                "spwd",
                "syslog",
                "termios",
                "xxlimited",
                "xxlimited_35",
            ],
        }
    }

    pub fn static_to_shared(&mut self, names: Vec<&'a str>) {
        for name in names {
            self.statik.retain(|&x| x != name);
            self.shared.push(name);
        }
    }

    pub fn shared_to_static(&mut self, names: Vec<&'a str>) {
        for name in names {
            self.shared.retain(|&x| x != name);
            self.statik.push(name);
        }
    }

    pub fn static_to_disabled(&mut self, names: Vec<&'a str>) {
        for name in names {
            self.statik.retain(|&x| x != name);
            self.disabled.push(name);
        }
    }

    pub fn disabled_to_static(&mut self, names: Vec<&'a str>) {
        for name in names {
            self.disabled.retain(|&x| x != name);
            self.statik.push(name);
        }
    }

    pub fn shared_to_disabled(&mut self, names: Vec<&'a str>) {
        for name in names {
            self.shared.retain(|&x| x != name);
            self.disabled.push(name);
        }
    }

    pub fn disabled_to_shared(&mut self, names: Vec<&'a str>) {
        for name in names {
            self.disabled.retain(|&x| x != name);
            self.shared.push(name);
        }
    }
}
