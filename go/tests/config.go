package config

import (
	"os"
	"runtime"
	"sort"
	"strings"
	"text/template"

	"github.com/charmbracelet/log"
	"gopkg.in/yaml.v3"
)

const PLATFORM = runtime.GOOS

type Config struct {
	Name     string
	Version  string
	Headers  []string
	Exts     map[string][]string
	Core     []string
	Static   []string
	Shared   []string
	Disabled []string
}

func NewConfig(name string, version string) *Config {
	return &Config{
		Name:    name,
		Version: version,
		Headers: []string{
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
		},
		Exts: map[string][]string{
			"_abc":     {"_abc.c"},
			"_asyncio": {"_asynciomodule.c"},
			"_bisect":  {"_bisectmodule.c"},
			"_blake2": {
				"_blake2/blake2module.c",
				"_blake2/blake2b_impl.c",
				"_blake2/blake2s_impl.c",
			},

			"_bz2": {
				"_bz2module.c",
				"-I$(BZIP2)/include",
				"-L$(BZIP2)/lib",
				"$(BZIP2)/lib/libbz2.a",
			},
			"_codecs":         {"_codecsmodule.c"},
			"_codecs_cn":      {"cjkcodecs/_codecs_cn.c"},
			"_codecs_hk":      {"cjkcodecs/_codecs_hk.c"},
			"_codecs_iso2022": {"cjkcodecs/_codecs_iso2022.c"},
			"_codecs_jp":      {"cjkcodecs/_codecs_jp.c"},
			"_codecs_kr":      {"cjkcodecs/_codecs_kr.c"},
			"_codecs_tw":      {"cjkcodecs/_codecs_tw.c"},
			"_collections":    {"_collectionsmodule.c"},
			"_contextvars":    {"_contextvarsmodule.c"},
			"_csv":            {"_csv.c"},
			"_ctypes": {
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
			},
			"_curses":       {"-lncurses", "-lncursesw", "-ltermcap", "_cursesmodule.c"},
			"_curses_panel": {"-lpanel", "-lncurses", "_curses_panel.c"},
			"_datetime":     {"_datetimemodule.c"},
			"_dbm":          {"_dbmmodule.c", "-lgdbm_compat", "-DUSE_GDBM_COMPAT"},
			"_decimal":      {"_decimal/_decimal.c", "-DCONFIG_64=1"},
			"_elementtree":  {"_elementtree.c"},
			"_functools": {
				"-DPy_BUILD_CORE_BUILTIN",
				"-I$(srcdir)/Include/internal",
				"_functoolsmodule.c",
			},
			"_gdbm": {"_gdbmmodule.c", "-lgdbm"},
			"_hashlib": {
				"_hashopenssl.c",
				"-I$(OPENSSL)/include",
				"-L$(OPENSSL)/lib",
				"$(OPENSSL)/lib/libcrypto.a",
			},
			"_heapq": {"_heapqmodule.c"},
			"_io": {
				"_io/_iomodule.c",
				"_io/iobase.c",
				"_io/fileio.c",
				"_io/bytesio.c",
				"_io/bufferedio.c",
				"_io/textio.c",
				"_io/stringio.c",
			},
			"_json":   {"_json.c"},
			"_locale": {"-DPy_BUILD_CORE_BUILTIN", "_localemodule.c"},
			"_lsprof": {"_lsprof.c", "rotatingtree.c"},
			"_lzma": {
				"_lzmamodule.c",
				"-I$(LZMA)/include",
				"-L$(LZMA)/lib",
				"$(LZMA)/lib/liblzma.a",
			},
			"_md5":            {"md5module.c"},
			"_multibytecodec": {"cjkcodecs/multibytecodec.c"},
			"_multiprocessing": {
				"_multiprocessing/multiprocessing.c",
				"_multiprocessing/semaphore.c",
			},
			"_opcode":          {"_opcode.c"},
			"_operator":        {"_operator.c"},
			"_pickle":          {"_pickle.c"},
			"_posixshmem":      {"_multiprocessing/posixshmem.c"},
			"_posixsubprocess": {"_posixsubprocess.c"},
			"_queue":           {"_queuemodule.c"},
			"_random":          {"_randommodule.c"},
			"_scproxy":         {"_scproxy.c"},
			"_sha1":            {"sha1module.c"},
			"_sha256":          {"sha256module.c"},
			"_sha3":            {"_sha3/sha3module.c"},
			"_sha512":          {"sha512module.c"},
			"_signal": {
				"-DPy_BUILD_CORE_BUILTIN",
				"-I$(srcdir)/Include/internal",
				"signalmodule.c",
			},
			"_socket": {"socketmodule.c"},
			"_sqlite3": {
				"_sqlite/blob.c",
				"_sqlite/connection.c",
				"_sqlite/cursor.c",
				"_sqlite/microprotocols.c",
				"_sqlite/module.c",
				"_sqlite/prepare_protocol.c",
				"_sqlite/row.c",
				"_sqlite/statement.c",
				"_sqlite/util.c",
			},
			"_sre": {"_sre/sre.c", "-DPy_BUILD_CORE_BUILTIN"},
			"_ssl": {
				"_ssl.c",
				"-I$(OPENSSL)/include",
				"-L$(OPENSSL)/lib",
				"$(OPENSSL)/lib/libcrypto.a",
				"$(OPENSSL)/lib/libssl.a",
			},
			"_stat":       {"_stat.c"},
			"_statistics": {"_statisticsmodule.c"},
			"_struct":     {"_struct.c"},
			"_symtable":   {"symtablemodule.c"},
			"_thread": {
				"-DPy_BUILD_CORE_BUILTIN",
				"-I$(srcdir)/Include/internal",
				"_threadmodule.c",
			},
			"_tracemalloc": {"_tracemalloc.c"},
			"_typing":      {"_typingmodule.c"},
			"_uuid":        {"_uuidmodule.c"},
			"_weakref":     {"_weakref.c"},
			"_zoneinfo":    {"_zoneinfo.c"},
			"array":        {"arraymodule.c"},
			"atexit":       {"atexitmodule.c"},
			"binascii":     {"binascii.c"},
			"cmath":        {"cmathmodule.c"},
			"errno":        {"errnomodule.c"},
			"faulthandler": {"faulthandler.c"},
			"fcntl":        {"fcntlmodule.c"},
			"grp":          {"grpmodule.c"},
			"itertools":    {"itertoolsmodule.c"},
			"math":         {"mathmodule.c"},
			"mmap":         {"mmapmodule.c"},
			"ossaudiodev":  {"ossaudiodev.c"},
			"posix": {
				"-DPy_BUILD_CORE_BUILTIN",
				"-I$(srcdir)/Include/internal",
				"posixmodule.c",
			},
			"pwd": {"pwdmodule.c"},
			"pyexpat": {
				"expat/xmlparse.c",
				"expat/xmlrole.c",
				"expat/xmltok.c",
				"pyexpat.c",
				"-I$(srcdir)/Modules/expat",
				"-DHAVE_EXPAT_CONFIG_H",
				"-DUSE_PYEXPAT_CAPI",
				"-DXML_DEV_URANDOM",
			},
			"readline": {"readline.c", "-lreadline", "-ltermcap"},
			"resource": {"resource.c"},
			"select":   {"selectmodule.c"},
			"spwd":     {"spwdmodule.c"},
			"syslog":   {"syslogmodule.c"},
			"termios":  {"termios.c"},
			"time": {
				"-DPy_BUILD_CORE_BUILTIN",
				"-I$(srcdir)/Include/internal",
				"timemodule.c",
			},
			"unicodedata": {"unicodedata.c"},
			"zlib":        {"zlibmodule.c", "-lz"},
		},

		Core: []string{
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
		},

		Static: []string{
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
		},

		Shared: []string{},

		Disabled: []string{
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
		},
	}
}

func (c *Config) Ver() string {
	return strings.Join(strings.Split(c.Version, ".")[:2], ".")
}

func (c *Config) StaticToShared(names ...string) {
	log.Debug("config.StaticToShared", "names", names)
	c.Static = RemoveNames("static", c.Static, names...)
	CheckRemoval("static", c.Static, names...)
	c.Shared = AddNames("shared", c.Shared, names...)
	CheckAddition("shared", c.Shared, names...)
}

func (c *Config) SharedToStatic(names ...string) {
	log.Debug("config.SharedToStatic", "names", names)
	c.Shared = RemoveNames("shared", c.Shared, names...)
	CheckRemoval("shared", c.Shared, names...)
	c.Static = AddNames("static", c.Static, names...)
	CheckAddition("static", c.Static, names...)
}

func (c *Config) StaticToDisabled(names ...string) {
	log.Debug("config.StaticToDisabled", "names", names)
	c.Static = RemoveNames("static", c.Static, names...)
	CheckRemoval("static", c.Static, names...)
	c.Disabled = AddNames("disabled", c.Disabled, names...)
	CheckAddition("disabled", c.Disabled, names...)
}

func (c *Config) SharedToDisabled(names ...string) {
	log.Debug("config.SharedToDisabled", "names", names)
	c.Shared = RemoveNames("shared", c.Shared, names...)
	CheckRemoval("shared", c.Shared, names...)
	c.Disabled = AddNames("disabled", c.Disabled, names...)
	CheckAddition("disabled", c.Disabled, names...)
}

func (c *Config) DisabledToStatic(names ...string) {
	log.Debug("config.DisabledToStatic", "names", names)
	c.Disabled = RemoveNames("disabled", c.Disabled, names...)
	CheckRemoval("disabled", c.Disabled, names...)
	c.Static = AddNames("static", c.Static, names...)
	CheckAddition("static", c.Static, names...)
}

func (c *Config) DisabledToShared(names ...string) {
	log.Debug("config.DisabledToShared", "names", names)
	c.Disabled = RemoveNames("disabled", c.Disabled, names...)
	CheckRemoval("disabled", c.Disabled, names...)
	c.Shared = AddNames("shared", c.Shared, names...)
	CheckAddition("shared", c.Shared, names...)
}

func (c *Config) Sort() {
	sort.Strings(c.Static)
	sort.Strings(c.Shared)
	sort.Strings(c.Disabled)
}

func (c *Config) Configure() {

	log.Debug("config.Configure: starting", "plat", PLATFORM, "cfg", c.Name, "ver", c.Version)

	if PLATFORM == "darwin" {
		log.Debug("config.Configure: common > darwin")
		c.DisabledToStatic("_scproxy")
	}
	if PLATFORM == "linux" {
		log.Debug("config.Configure: common > linux")

		c.DisabledToStatic("ossaudiodev")

		c.Exts["_ssl"] = []string{
			"_ssl.c",
			"-I$(OPENSSL)/include",
			"-L$(OPENSSL)/lib",
			"-l:libssl.a -Wl,--exclude-libs,libssl.a",
			"-l:libcrypto.a -Wl,--exclude-libs,libcrypto.a",
		}
		c.Exts["_hashlib"] = []string{
			"_hashopenssl.c",
			"-I$(OPENSSL)/include",
			"-L$(OPENSSL)/lib",
			"-l:libcrypto.a -Wl,--exclude-libs,libcrypto.a",
		}
	}

	if c.Ver() == "3.11" {

		if c.Name == "static_max" {
			log.Debug("config.Configure: 3.11 -> static_max")
			if PLATFORM == "linux" {
				c.StaticToDisabled("_decimal")
			}

		} else if c.Name == "static_mid" {
			log.Debug("config.Configure: 3.11 -> static_mid")
			c.StaticToDisabled("_decimal")

		} else if c.Name == "static_min" {
			log.Debug("config.Configure: 3.11 -> static_min")
			c.StaticToDisabled("_bz2", "_decimal", "_csv", "_json",
				"_lzma", "_scproxy", "_sqlite3", "_ssl", "pyexpat",
				"readline")

		} else if c.Name == "shared_max" {
			log.Debug("config.Configure: 3.11 -> shared_max")
			if PLATFORM == "linux" {
				c.StaticToDisabled("_decimal")
			} else {
				c.DisabledToShared("_ctypes")
				c.StaticToShared("_decimal", "_ssl", "_hashlib")
			}

		} else if c.Name == "shared_mid" {
			log.Debug("config.Configure: 3.11 -> shared_mid")
			c.StaticToDisabled("_decimal", "_ssl", "_hashlib")

		}

	} else if c.Ver() == "3.12" {

		c.Exts["_md5"] = []string{
			"md5module.c",
			"-I$(srcdir)/Modules/_hacl/include",
			"_hacl/Hacl_Hash_MD5.c",
			"-D_BSD_SOURCE",
			"-D_DEFAULT_SOURCE",
		}

		c.Exts["_sha1"] = []string{
			"sha1module.c",
			"-I$(srcdir)/Modules/_hacl/include",
			"_hacl/Hacl_Hash_SHA1.c",
			"-D_BSD_SOURCE",
			"-D_DEFAULT_SOURCE",
		}

		c.Exts["_sha2"] = []string{
			"sha2module.c",
			"-I$(srcdir)/Modules/_hacl/include",
			"_hacl/Hacl_Hash_SHA2.c",
			"-D_BSD_SOURCE",
			"-D_DEFAULT_SOURCE",
			"Modules/_hacl/libHacl_Hash_SHA2.a",
		}

		c.Exts["_sha3"] = []string{
			"sha3module.c",
			"-I$(srcdir)/Modules/_hacl/include",
			"_hacl/Hacl_Hash_SHA3.c",
			"-D_BSD_SOURCE",
			"-D_DEFAULT_SOURCE",
		}

		delete(c.Exts, "_sha256")
		delete(c.Exts, "_sha512")

		c.Static = Append(c.Static, "_sha2")
		c.Disabled = Append(c.Disabled, "_xxinterpchannels")

		c.Static = RemoveNames("static", c.Static, "_sha256", "_sha512")

		if c.Name == "static_max" {
			log.Debug("config.Configure: 3.12 -> static_max")
			if PLATFORM == "linux" {
				log.Debug("config.Configure: 3.12 > static_max > linux")
				c.StaticToDisabled("_decimal")
			}
		} else if c.Name == "static_mid" {
			log.Debug("config.Configure: 3.12 -> static_mid")
			c.StaticToDisabled("_decimal")

		} else if c.Name == "static_min" {
			log.Debug("config.Configure: 3.12 > static_min")
			c.StaticToDisabled("_bz2", "_decimal", "_csv", "_json",
				"_lzma", "_scproxy", "_sqlite3", "_ssl", "pyexpat",
				"readline")

		} else if c.Name == "shared_max" {
			log.Debug("config.Configure: 3.12 -> shared_max")
			c.DisabledToShared("_ctypes")
			c.StaticToShared("_decimal", "_ssl", "_hashlib")

		} else if c.Name == "shared_mid" {
			log.Debug("config.Configure: 3.12 -> shared_max")
			c.StaticToDisabled("_decimal", "_ssl", "_hashlib")
		}
	}
}

func (c *Config) ToSetupLocal() *template.Template {
	c.Sort()
	funcMap := template.FuncMap{
		"join": strings.Join,
	}

	tmpl, err := template.New("test").Funcs(funcMap).Parse(Template)
	if err != nil {
		log.Fatal(err)
	}
	return tmpl
}

func (c *Config) PrintSetupLocal() {
	tmpl := c.ToSetupLocal()
	err := tmpl.Execute(os.Stdout, c)
	if err != nil {
		log.Fatal(err)
	}
}

func (c *Config) WriteSetupLocal(path string) {
	tmpl := c.ToSetupLocal()
	f, err := os.Create(path)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	err = tmpl.Execute(f, c)
	if err != nil {
		log.Fatal(err)
	}
}

func (c *Config) FromYaml(path string) {

	data, err := os.ReadFile(path)
	if err != nil {
		log.Fatal(err)
	}

	err = yaml.Unmarshal(data, c)
	if err != nil {
		log.Fatal(err)
	}
}

func (c *Config) ToYaml() string {
	c.Sort()
	data, err := yaml.Marshal(&c)
	if err != nil {
		log.Fatalf("error: %v", err)
	}
	return string(data)
}

func (c *Config) PrintYaml() {
	data := c.ToYaml()
	log.Printf("\n%s\n", data)
}

func (c *Config) WriteYaml(path string) {
	data := c.ToYaml()

	f, err := os.Create(path)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	size, err := f.WriteString(data)
	if err != nil {
		log.Fatal(err)
	}
	log.Info("wrote yaml", "file", path, "size", size)
}
