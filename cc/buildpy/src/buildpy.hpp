/* buildpy - build python3 from source

ShellCmd
    Project
    Builder
        OpenSSLBuilder
        Bzip2Builder
        XzBuilder
        PythonBuilder
*/

#include <argparse/argparse.hpp>
#include <fmt/core.h>
#include <glob/glob.hpp>
#include <logy.h>
#include <semver.hpp>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "config.hpp"

#define BUFFERSIZE 4096
#define USE_GIT 0


namespace fs = std::filesystem;

class ShellCmd {
    // Utility class to hold common file operations
public:
    void cmd_exe(std::string exe, std::vector<std::string> args,
                 fs::path dir = ".")
    {
        args.insert(args.begin(), exe);
        this->cmd(args, dir);
    }

    void cmd(std::vector<std::string> args, fs::path dir = ".")
    {
        fs::path cwd;
        if (dir != ".") {
            cwd = fs::current_path();
            fs::current_path(dir);
        }
        std::string _cmd = this->join(args, " ");
        const char* scmd = _cmd.c_str();

        Info("%s", scmd);
        if (std::system(scmd) != 0) {
            Error("failed: %s", scmd);
            return;
        }
        if (dir != ".")
            fs::current_path(cwd);
    }

    void run(std::string shellcmd, fs::path dir = ".")
    {
        fs::path cwd;
        const char* scmd = shellcmd.c_str();

        if (dir != ".") {
            cwd = fs::current_path();
            fs::current_path(dir);
        }
        Info("%s", scmd);
        if (std::system(scmd) != 0) {
            Error("failed: %s", scmd);
            return;
        }
        if (dir != ".") {
            fs::current_path(cwd);
        }
    }

    void run_list(std::initializer_list<std::string> args, fs::path dir = ".")
    {
        std::vector<std::string> vargs = std::vector(args);
        this->cmd(vargs, dir);
    }

    std::vector<std::string> split(std::string s, char delimiter)
    {
        std::vector<std::string> output;
        for (auto cur = std::begin(s), beg = cur;; ++cur) {
            if (cur == std::end(s) || *cur == delimiter || !*cur) {
                output.insert(output.end(), std::string(beg, cur));
                if (cur == std::end(s) || !*cur)
                    break;
                else
                    beg = std::next(cur);
            }
        }
        return output;
    }


    std::string join(std::vector<std::string> elements,
                     const char* const delimiter)
    {
        std::ostringstream os;
        auto b = std::begin(elements);
        auto e = std::end(elements);

        if (b != e) {
            std::copy(b, std::prev(e),
                      std::ostream_iterator<std::string>(os, delimiter));
            b = std::prev(e);
        }
        if (b != e) {
            os << *b;
        }

        return os.str();
    }

    void create_dir(fs::path p)
    {
        if (!fs::exists(p)) {
            fs::create_directory(p);
        }
    }

    void wget(std::string url, fs::path download_dir, fs::path cwd)
    {
        std::string _cmd = fmt::format("wget -P {} {}", download_dir.string(),
                                       url);
        this->run(_cmd, cwd.string());
    }

    void curl(std::string url, fs::path download_dir, fs::path cwd)
    {
        std::string _cmd = fmt::format("curl -L --output-dir {} -O {}",
                                       download_dir.string(), url);
        this->run(_cmd, cwd.string());
    }

    void tar(fs::path archive, fs::path srcdir)
    {
        if (!fs::exists(srcdir)) {
            this->create_dir(srcdir);
        }
        std::string _cmd = fmt::format("tar xvf {} -C {} --strip-components=1",
                                       archive.string(), srcdir.string());
        this->run(_cmd, ".");
    }

    void git_clone(std::string url, std::string branch, fs::path dir,
                   bool recurse = false)
    {
        if (fs::exists(dir)) {
            Warning("skipping git clone, dir exists: %s", dir.c_str());
            return;
        }
        std::vector<std::string> args;
        args.insert(args.end(), { "clone", "--depth=1", "-b", branch });
        if (recurse) {
            args.insert(
                args.end(),
                { "--recurse-submodules", "--shallow-submodules", url, dir });
        } else {
            args.insert(args.end(), { url, dir });
        }
        this->cmd_exe("/usr/bin/git", args);
    }

    void cmake_configure(fs::path srcdir, fs::path builddir,
                         std::string options = "")
    {
        std::string _cmd = fmt::format("cmake -S {} -B {} {}", srcdir.string(),
                                       builddir.string(), options);
        this->run(_cmd, ".");
    }

    void cmake_build(fs::path builddir, bool release = false)
    {
        std::string release_stmt = release ? "--config Release" : "";
        std::string _cmd = fmt::format("cmake --build {} {}",
                                       builddir.string(), release_stmt);
        this->run(_cmd, ".");
    }

    void cmake_install(fs::path builddir, fs::path prefix)
    {
        std::string _cmd = fmt::format("cmake --install {} --prefix {}",
                                       builddir.string(), prefix.string());
        this->run(_cmd, ".");
    }

    std::vector<std::string> split(std::string s, std::string sep)
    {
        std::vector<std::string> res;
        int pos = 0;
        while (pos < s.size()) {
            pos = s.find(sep);
            res.push_back(s.substr(0, pos));
            // length of the seperator, sep
            s.erase(0, pos + sep.size());
        }
        return res;
    }

    std::string join(const std::vector<std::string>& lst,
                     const std::string& delim)
    {
        std::string ret;
        for (const auto& s : lst) {
            if (!ret.empty())
                ret += delim;
            ret += s;
        }
        return ret;
    }

    void remove(fs::path target)
    {
        if (fs::exists(target)) {
            if (fs::remove_all(target)) {
                Info("removed: %s", target.c_str());
            }
        }
    }

    void move(fs::path& src, fs::path& dst)
    {
        Info("move %s to %s", src.c_str(), dst.c_str());
        fs::rename(src, dst);
    }

    bool globmatch(fs::path name, std::vector<std::string> patterns)
    {
        for (auto& p : patterns) {
            if (glob::fnmatch(name, p)) {
                return true;
            }
        }
        return false;
    }

    void zip(fs::path zip_path, fs::path cwd)
    {
        std::string _cmd = fmt::format("zip -r {} .", zip_path.string());

        this->run(_cmd, cwd);
    }
};


class Project : public ShellCmd {
    // Utility class to hold project directory structure
public:
    // -----------------------------------------------------------------------
    // attributes

    fs::path cwd;
    fs::path build;
    fs::path downloads;
    fs::path src;
    fs::path install;

    // -----------------------------------------------------------------------
    // constructor

    Project()
    {
        this->cwd = fs::current_path();
        this->build = this->cwd / "build";
        this->downloads = this->build / "downloads";
        this->src = this->build / "src";
        this->install = this->build / "install";
    }

    // -----------------------------------------------------------------------
    // methods

    void setup()
    {
        this->create_dir(this->build);
        this->create_dir(this->downloads);
        this->create_dir(this->install);
        this->create_dir(this->src);
    }
};


class Builder : public ShellCmd {

public:
    // -----------------------------------------------------------------------
    // operators

    virtual void operator()()
    {
        // can be used in taskflow
        this->process();
    }

    // -----------------------------------------------------------------------
    // predicates

    virtual bool libs_exist()
    {
        return fs::exists(this->staticlib()) || fs::exists(this->dylib());
    }

    // -----------------------------------------------------------------------
    // properties

    virtual semver::version version() const = 0;

    virtual std::string name() const = 0;

    virtual std::string repo_url() const = 0;

    virtual std::string repo_branch() const = 0;

    virtual std::string download_url() const = 0;

    virtual std::string archive_name() const = 0;

    virtual fs::path archive()
    {
        return this->project().downloads / this->archive_name();
    }

    virtual Project project() const = 0;

    virtual fs::path src_dir() { return this->project().src / this->name(); }

    virtual fs::path build_dir()
    {
        return this->src_dir() / std::string("build");
    }

    virtual fs::path prefix()
    {
        return this->project().install / this->name();
    }

    virtual std::string staticlib_name()
    {
        return fmt::format("lib{}.a", this->name());
    }

    virtual fs::path staticlib()
    {
        return this->prefix() / "lib" / this->staticlib_name();
    }

    virtual std::string dylib_name()
    {
        return fmt::format("lib{}.dylib", this->name());
    }

    virtual fs::path dylib()
    {
        return this->prefix() / "lib" / this->dylib_name();
    }

    // -----------------------------------------------------------------------
    // methods

    virtual void download()
    {
        if (USE_GIT) {
            this->git_clone(this->repo_url(), this->repo_branch(),
                            this->src_dir());
        } else {
            fs::path downloads = this->project().downloads;
            if (!fs::exists(this->archive())) {
                this->wget(this->download_url(), downloads, ".");
            }
            this->tar(this->archive(), this->src_dir());
        }
    }

    virtual void process() = 0;
};


class OpenSSLBuilder : public Builder {
    // builds openssl from source
private:
    semver::version _version;
    std::string _name;
    std::string _repo_url;
    Project _project;

public:
    // -----------------------------------------------------------------------
    // constructor

    OpenSSLBuilder(std::string version)
    {
        this->_version = semver::version::parse(version);
        this->_name = "openssl";
        this->_repo_url = "https://github.com/openssl/openssl.git";
        this->_project = Project();
    }

    // -----------------------------------------------------------------------
    // properties

    semver::version version() const { return this->_version; }

    std::string name() const { return this->_name; }

    std::string repo_url() const { return this->_repo_url; }

    std::string archive_name() const
    {
        return fmt::format("openssl-{}w.tar.gz", this->version().str());
    }

    Project project() const { return this->_project; }

    std::string repo_branch() const
    {
        std::string s = this->version().str();
        std::replace(s.begin(), s.end(), '.', '_'); // replace all '.' to '_'
        return fmt::format("OpenSSL_{}w", s);
    }

    std::string download_url() const
    {
        return fmt::format(
            "https://www.openssl.org/source/old/1.1.1/openssl-{}w.tar.gz",
            this->version().str());
    }

    std::string staticlib_name()
    {
        return "libssl.a";
    }

    std::string dylib_name()
    {
        return "libssl.dylib";
    }

    // -----------------------------------------------------------------------
    // methods

    void setup() { this->project().setup(); }

    void build()
    {
        Info("OpenSSLBuilder.build()");
        if (!this->libs_exist()) {
            std::string _cmd = fmt::format(
                "/bin/bash ./config no-shared no-tests --prefix={}",
                this->prefix().string());
            this->run(_cmd, this->src_dir());
            this->run("make install_sw", this->src_dir());
        }
    }

    void process()
    {
        Info("OpenSSLBuilder.process() start");
        this->setup();
        this->download();
        this->build();
        Info("OpenSSLBuilder.process() end");
    }
};


class Bzip2Builder : public Builder {
    // builds bzip2 from source

private:
    semver::version _version;
    std::string _name;
    std::string _repo_url;
    Project _project;

public:
    // -----------------------------------------------------------------------
    // constructor

    Bzip2Builder(std::string version)
    {
        this->_version = semver::version::parse(version);
        this->_name = "bzip2";
        this->_repo_url = "https://github.com/libarchive/bzip2.git";
        this->_project = Project();
    }

    // -----------------------------------------------------------------------
    // properties

    semver::version version() const { return this->_version; }

    std::string name() const { return this->_name; }

    std::string repo_url() const { return this->_repo_url; }

    Project project() const { return this->_project; }

    std::string repo_branch() const
    {
        return fmt::format("bzip2-{}", this->version().str());
    }

    std::string download_url() const
    {
        return fmt::format("https://sourceware.org/pub/bzip2/bzip2-{}.tar.gz",
                           this->version().str());
    }

    std::string archive_name() const
    {
        return fmt::format("bzip2-{}.tar.gz", this->version().str());
    }

    // -----------------------------------------------------------------------
    // methods

    void setup() { this->project().setup(); }

    void build()
    {
        Info("Bzip2Builder.build()");
        if (!this->libs_exist()) {
            std::string _cmd = fmt::format(
                "make install CFLAGS='-fPIC' PREFIX={}",
                this->prefix().string());
            this->run(_cmd, this->src_dir());
        }
    }

    void process()
    {
        Info("Bzip2Builder process starting");
        this->setup();
        this->download();
        this->build();
        Info("Bzip2Builder process ending");
    }
};


class XzBuilder : public Builder {
    // builds xz from source

private:
    semver::version _version;
    std::string _name;
    std::string _repo_url;
    Project _project;

public:
    // -----------------------------------------------------------------------
    // constructor

    XzBuilder(std::string version)
    {
        this->_version = semver::version::parse(version);
        this->_name = "xz";
        this->_repo_url = "https://github.com/tukaani-project/xz.git";
        this->_project = Project();
    }


    // -----------------------------------------------------------------------
    // properties

    semver::version version() const { return this->_version; }

    std::string name() const { return this->_name; }

    std::string repo_url() const { return this->_repo_url; }

    Project project() const { return this->_project; }

    std::string repo_branch() const
    {
        return fmt::format("v{}", this->version().str());
    }

    std::string download_url() const
    {
        return fmt::format("https://github.com/tukaani-project/xz/releases/"
                           "download/v{}/xz-{}.tar.gz",
                           this->version().str(), this->version().str());
    }

    std::string archive_name() const
    {
        return fmt::format("xz-{}.tar.gz", this->version().str());
    }

    // -----------------------------------------------------------------------
    // methods

    void setup() { this->project().setup(); }

    void build()
    {
        Info("XzBuilder.build()");

        if (!this->libs_exist()) {
            putenv((char*)"CFLAGS=-fPIC");
            this->cmake_configure(
                this->src_dir(), this->build_dir(),
                "-DBUILD_SHARED_LIBS=OFF -DENABLE_NLS=OFF -DENABLE_SMALL=ON "
                "-DCMAKE_BUILD_TYPE=MinSizeRel");
            this->cmake_build(this->build_dir());
            this->cmake_install(this->build_dir(), this->prefix());
        }
    }

    void process()
    {
        Info("XzBuilder process starting");
        this->setup();
        this->download();
        this->build();
        Info("XzBuilder process ending");
    }
};


class PythonBuilder : public Builder {
    // builds python from source

private:
    semver::version _version;
    std::string _config;
    bool _optimize;
    std::vector<std::string> _options;
    std::string _name;
    std::string _repo_url;
    Project _project;
    std::string _build_type;
    std::string _size_type;

public:
    // -----------------------------------------------------------------------
    // constructor

    PythonBuilder(std::string version, std::string config,
                  bool optimize = false)
    {
        this->_version = semver::version::parse(version);
        this->_config = config;
        this->_optimize = optimize;
        this->_name = "python";
        this->_repo_url = "https://github.com/python/cpython.git";
        this->_project = Project();
        std::vector<std::string> parsed = this->split(config, '_');
        this->_build_type = parsed[0];
        this->_size_type = parsed[1];
    }

    // -----------------------------------------------------------------------
    // properties

    semver::version version() const { return this->_version; }

    std::string name() const { return this->_name; }

    std::string repo_url() const { return this->_repo_url; }

    Project project() const { return this->_project; }

    std::string repo_branch() const
    {
        return fmt::format("v{}", this->version().str());
    }

    std::string build_type() const { return this->_build_type; }

    std::string size_type() const { return this->_size_type; }

    std::string ver()
    {
        return fmt::format("{}.{}", this->version().major(),
                           this->version().minor());
    }

    std::string ver_nodot()
    {
        return fmt::format("{}{}", this->version().major(),
                           this->version().minor());
    }

    std::string name_version()
    {
        return fmt::format("{}{}", this->name(), this->version().str());
    }

    std::string name_ver()
    {
        return fmt::format("{}{}", this->name(), this->ver());
    }

    std::string download_url() const
    {
        return fmt::format(
            "https://www.python.org/ftp/python/{}/Python-{}.tar.xz",
            this->version().str(), this->version().str());
    }

    std::string archive_name() const
    {
        return fmt::format("Python-{}.tar.xz", this->version().str());
    }

    std::string executable_name() { return fmt::format("{}3", this->name()); }

    fs::path executable()
    {
        return this->project().install / "bin" / this->executable_name();
    }

    // std::string dylib_link_name() = 0;
    // fs::path dylib_link() = 0;

    std::string staticlib_name()
    {
        return fmt::format("lib{}.a", this->name_ver());
    }

    std::string dylib_name()
    {
        return fmt::format("lib{}.dylib",  this->name_ver());
    }

    // -----------------------------------------------------------------------
    // methods

    void preprocess() { }

    void install_dependencies()
    {
        Info("PythonBuilder.install_dependencies()");
        OpenSSLBuilder("1.1.1").process();
        Bzip2Builder("1.0.8").process();
        XzBuilder("5.6.0").process();
    }

    void info()
    {
        Info("ver: %s", this->ver().c_str());
        Info("name_ver: %s", this->name_ver().c_str());
        Info("prefix: %s", this->prefix().c_str());
    }

    void setup()
    {
        Info("PythonBuilder.setup()");
        this->project().setup();
        this->install_dependencies();
        this->download();
    }

    void configure()
    {
        Info("PythonBuilder.configure()");

        std::string _prefix = fmt::format("--prefix={}",
                                          this->prefix().string());
        std::vector<std::string> args = {
            "/bin/bash",
            "./configure",
            "--disable-test-modules",
            "--without-ensurepip",
        };


        if (this->_build_type == "shared") {
            args.insert(args.end(),
                        { "--enable-shared", "--without-static-libpython" });
        }

        if (this->_optimize) {
            args.push_back("--enable-optimizations");
        }

        args.push_back(_prefix);

        this->cmd(args, this->src_dir());
    }

    void build()
    {
        Info("PythonBuilder.build()");
        this->run("make", this->src_dir());
    }

    void install()
    {
        Info("PythonBuilder.install()");
        this->run("make install", this->src_dir());
    }

    void clean()
    {
        Info("PythonBuilder.clean()");
        std::vector<std::string> patterns = {
            "**/*.exe",         "**/*config-3*",   "**/*tcl*",
            "**/*tdbc*",        "**/*tk*",         "**/__phello__",
            "**/__pycache__",   "**/_codecs_*.so", "**/_ctypes_test*",
            "**/_test*",        "**/_tk*",         "**/_xx*.so",
            "**/distutils",     "**/idlelib",      "**/lib2to3",
            "**/LICENSE.txt",   "**/pkgconfig",    "**/pydoc_data",
            "**/site-packages", "**/test",         "**/Tk*",
            "**/turtle*",       "**/venv",         "**/xx*.so",
        };
        fs::path cwd = fs::current_path();
        fs::current_path(this->prefix());
        for (auto& p : glob::rglob(patterns)) {
            this->remove(p);
        }
        fs::current_path(cwd);
    }

    void ziplib()
    {
        Info("PythonBuilder.ziplib()");

        fs::path tmp_libdynload = this->project().build / "lib-dynload";
        fs::path tmp_os_py = this->project().build / "os.py";

        // pre-cleanup

        this->remove(tmp_libdynload);
        this->remove(tmp_os_py);

        fs::path src = this->prefix() / "lib" / this->name_ver();

        fs::path src_libdynload = src / "lib-dynload";
        fs::path src_os_py = src / "os.py";

        this->move(src_libdynload, tmp_libdynload);
        this->move(src_os_py, tmp_os_py);

        std::string zipfile = fmt::format("python{}.zip", this->ver_nodot());
        fs::path zip_path = this->prefix() / "lib" / zipfile;

        this->zip(zip_path, src);

        this->remove(src);

        fs::path site_packages = src / "site-packages";

        fs::path pkgconfig = this->prefix() / "lib" / "pkgconfig";

        this->remove(pkgconfig);

        this->create_dir(src);           // perm 0750
        this->create_dir(site_packages); // perm 0750

        this->move(tmp_libdynload, src_libdynload);
        this->move(tmp_os_py, src_os_py);
    }

    void postprocess()
    {
        Info("PythonBuilder.postprocess()");
        ziplib();
    }

    void process()
    {
        Info("PythonBuilder process starting");
        if (!this->libs_exist()) {
            this->preprocess();
            // this->download();
            this->setup();
            this->configure();
            this->build();
            this->install();
            this->clean();
            this->postprocess();
        } else {
            Warning("%s already built", this->name().c_str());
        }
        Info("PythonBuilder process ending");
    }
};
