#include <argparse/argparse.hpp>
#include <fmt/core.h>
#include <logy.h>
#include <semver.hpp>
#include <subprocess.h>
#include <taskflow/taskflow.hpp>

#include <algorithm>
#include <initializer_list>
#include <string>
#include <vector>

#include <chrono>
#include <indicators/indicators.hpp>
#include <thread>


int wait2(std::string text)
{
    indicators::IndeterminateProgressBar bar {
        indicators::option::BarWidth { 40 }, indicators::option::Start { "[" },
        indicators::option::Fill { "·" },
        // indicators::option::Lead { "<==>" },
        indicators::option::Lead { "<==>" }, indicators::option::End { "]" },
        indicators::option::PostfixText { text },
        indicators::option::ForegroundColor { indicators::Color::yellow },
        indicators::option::FontStyles { std::vector<indicators::FontStyle> {
            indicators::FontStyle::bold } }
    };

    indicators::show_console_cursor(false);

    auto job = [&bar]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        bar.mark_as_completed();
        std::cout << termcolor::bold << termcolor::green << "DONE!"
                  << termcolor::reset << std::endl;
    };
    std::thread job_completion_thread(job);

    // Update bar state
    while (!bar.is_completed()) {
        bar.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    job_completion_thread.join();

    indicators::show_console_cursor(true);
    return 0;
}


int wait(std::string text)
{
    using namespace indicators;
    indicators::ProgressSpinner spinner {
        option::PostfixText { text },
        option::ForegroundColor { Color::yellow },
        // option::ShowPercentage { false },
        // option::ShowElapsedTime { true },
        option::SpinnerStates { std::vector<std::string> {
            "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁" } },
        option::FontStyles { std::vector<FontStyle> { FontStyle::bold } }
    };

    // Update spinner state
    auto job = [&spinner]() {
        while (true) {
            if (spinner.is_completed()) {
                spinner.set_option(option::ForegroundColor { Color::green });
                spinner.set_option(option::PrefixText { "✔" });
                spinner.set_option(option::ShowSpinner { false });
                spinner.set_option(option::ShowPercentage { false });
                spinner.set_option(option::PostfixText { "DONE!" });
                spinner.mark_as_completed();
                break;
            } else
                spinner.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    };
    std::thread thread(job);
    thread.join();

    return 0;
}

namespace fs = std::filesystem;


class ShellCmd {
    // Utility class to hold common file operations
public:
    void cmd(std::vector<std::string> args)
    {
        std::vector<char*> vc;

        std::transform(args.begin(), args.end(), std::back_inserter(vc),
                       [](const std::string& s) -> char* {
                           char* pc = new char[s.size() + 1];
                           std::strcpy(pc, s.c_str());
                           return pc;
                       });

        struct subprocess_s subprocess;
        int result = subprocess_create(&vc[0], 0, &subprocess);
        if (0 != result) {
            Error("error occurred.\n");
            FILE* p_stdout = subprocess_stdout(&subprocess);
            char data[4096];
            fgets(data, 4096, p_stdout);            
        }
    }

    void run(std::initializer_list<std::string> args)
    {
        std::vector<std::string> vargs = std::vector(args);
        this->cmd(vargs);
    }

    void create_dir(fs::path p)
    {
        if (!fs::exists(p)) {
            fs::create_directory(p);
        }
    }

    void git_clone(std::string url, std::string branch, std::string dir,
                   bool recurse = false)
    {
        std::vector<std::string> args;
        args.insert(
            args.end(),
            { "/usr/bin/git", "clone", "--depth=1", "--branch", branch });
        if (recurse) {
            args.insert(
                args.end(),
                { "--recurse-submodules", "--shallow-submodules", url, dir });
        } else {
            args.insert(args.end(), { url, dir });
        }
        this->cmd(args);
        wait("git clone openssl");
    }

    std::vector<std::string> split(std::string s, std::string sep)
    {
        std::vector<std::string> res;
        int pos = 0;
        while (pos < s.size()) {
            pos = s.find(sep);
            res.push_back(s.substr(0, pos));
            s.erase(0,
                    pos + sep.size()); // length of the seperator, sep
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
        this->downloads = this->build / "build";
        this->src = this->build / "src";
        this->install = this->build / "install";
    }

    // -----------------------------------------------------------------------
    // methods

    void setup()
    {
        // create main project directories
        this->create_dir(this->build);
        this->create_dir(this->downloads);
        this->create_dir(this->install);
        this->create_dir(this->src);
    }
};


class OpenSSLBuilder : public ShellCmd {
    // builds openssl from source
public:
    semver::version version;
    std::string name;
    std::string repo_url;
    Project project;

    // -----------------------------------------------------------------------
    // constructor

    OpenSSLBuilder(std::string version)
    {
        this->version = semver::version::parse(version);
        this->name = "openssl";
        this->repo_url = "https://github.com/openssl/openssl.git";
        this->project = Project();
    }

    // -----------------------------------------------------------------------
    // operators

    void operator()()
    {
        // can be used in taskflow
        this->process();
    }

    // -----------------------------------------------------------------------
    // properties


    fs::path src_dir() { return this->project.src / this->name; }

    fs::path build_dir() { return this->src_dir() / std::string("build"); }

    fs::path prefix() { return this->project.install / this->name; }

    std::string repo_branch()
    { // "OpenSSL_1_1_1w"
        std::string s = this->version.str();
        std::replace(s.begin(), s.end(), '.', '_'); // replace all '.' to '_'
        return fmt::format("OpenSSL_{}w", s);
    }

    std::string download_url()
    {
        return fmt::format(
            "https://www.openssl.org/source/old/1.1.1/openssl-{}w.tar.gz",
            this->version.str());
    }

    // -----------------------------------------------------------------------
    // methods

    void process()
    {
        Info("OpenSSLBuilder process starting");
        std::string dir = (this->project.src / this->name).string();
        this->git_clone(this->repo_url, this->repo_branch(), dir);
        Info("OpenSSLBuilder process ending");
    }
};


class Bzip2Builder : public ShellCmd {
    // builds bzip2 from source
public:
    semver::version version;
    std::string name;
    std::string repo_url;
    Project project;

    // -----------------------------------------------------------------------
    // constructor

    Bzip2Builder(std::string version)
    {
        this->version = semver::version::parse(version);
        this->name = "bzip2";
        this->repo_url = "https://github.com/libarchive/bzip2.git";
        this->project = Project();
    }

    // -----------------------------------------------------------------------
    // operators

    void operator()()
    {
        // can be used in taskflow
        this->process();
    }

    // -----------------------------------------------------------------------
    // properties


    fs::path src_dir() { return this->project.src / this->name; }

    fs::path build_dir() { return this->src_dir() / std::string("build"); }

    fs::path prefix() { return this->project.install / this->name; }

    std::string repo_branch()
    {
        return fmt::format("bzip2-{}", this->version.str());
    }

    std::string download_url()
    {
        return fmt::format("https://sourceware.org/pub/bzip2/bzip2-{}.tar.gz",
                           this->version.str());
    }

    // -----------------------------------------------------------------------
    // methods

    void process()
    {
        Info("Bzip2Builder process starting");
        Info("Bzip2Builder process ending");
    }
};


class XzBuilder : public ShellCmd {
    // builds xz from source
public:
    semver::version version;
    std::string name;
    std::string repo_url;
    Project project;

    // -----------------------------------------------------------------------
    // constructor

    XzBuilder(std::string version)
    {
        this->version = semver::version::parse(version);
        this->name = "xz";
        this->repo_url = "https://github.com/tukaani-project/xz.git";
        this->project = Project();
    }

    // -----------------------------------------------------------------------
    // operators

    void operator()()
    {
        // can be used in taskflow
        this->process();
    }

    // -----------------------------------------------------------------------
    // properties


    fs::path src_dir() { return this->project.src / this->name; }

    fs::path build_dir() { return this->src_dir() / std::string("build"); }

    fs::path prefix() { return this->project.install / this->name; }

    std::string repo_branch()
    {
        return fmt::format("v{}", this->version.str());
    }

    std::string download_url()
    {
        return fmt::format("https://github.com/tukaani-project/xz/releases/"
                           "download/v{}/xz-{}.tar.gz",
                           this->version.str(), this->version.str());
    }

    // -----------------------------------------------------------------------
    // methods

    void process()
    {
        Info("XzBuilder process starting");
        Info("XzBuilder process ending");
    }
};


class PythonBuilder : public ShellCmd {
    // builds python from source
public:
    // -----------------------------------------------------------------------
    // attributes

    semver::version version;
    std::string name;
    std::string repo_url;
    Project project;

    // -----------------------------------------------------------------------
    // constructor

    PythonBuilder(std::string version)
    {
        this->version = semver::version::parse(version);
        this->name = "python";
        this->repo_url = "https://github.com/python/cpython.git";
        this->project = Project();
    }

    // -----------------------------------------------------------------------
    // operators

    void operator()()
    {
        // can be used in taskflow
        this->process();
    }

    // -----------------------------------------------------------------------
    // properties

    std::string ver()
    {
        return fmt::format("{}.{}", this->version.major(),
                           this->version.minor());
    }

    std::string ver_nodot()
    {
        return fmt::format("{}{}", this->version.major(),
                           this->version.minor());
    }

    std::string name_version()
    {
        return fmt::format("{}{}", this->name, this->version.str());
    }

    std::string name_ver()
    {
        return fmt::format("{}{}", this->name, this->ver());
    }

    fs::path src_dir() { return this->project.src / this->name; }

    fs::path build_dir() { return this->src_dir() / std::string("build"); }

    fs::path prefix() { return this->project.install / this->name; }

    std::string repo_branch()
    {
        return fmt::format("v{}", this->version.str());
    }

    std::string download_url()
    {
        return fmt::format(
            "https://www.python.org/ftp/python/{}/Python-{}.tar.xz",
            this->version.str(), this->version.str());
    }

    std::string executable_name()
    {
        // return fmt::format("python{}", this->version.str());
        return fmt::format("{}3", this->name);
    }

    fs::path executable()
    {
        return this->project.install / "bin" / this->executable_name();
    }

    std::string lib_name()
    {
        return fmt::format("lib{}{}", this->name, this->version.str());
    }

    std::string staticlib_name()
    {
        return fmt::format("{}.a", this->lib_name());
    }

    fs::path staticlib()
    {
        return this->project.install / "lib" / this->staticlib_name();
    }

    std::string dylib_name()
    {
        return fmt::format("{}.dylib", this->lib_name());
    }

    fs::path dylib()
    {
        return this->project.install / "lib" / this->dylib_name();
    }

    // std::string dylib_link_name() = 0;
    // fs::path dylib_link() = 0;

    // predicates
    bool libs_exist()
    {
        return fs::exists(this->staticlib()) || fs::exists(this->dylib());
    }

    // -----------------------------------------------------------------------
    // actions

    void preprocess() { }
    void setup() { }
    void configure() { }
    void build() { }
    void install() { }
    void clean() { }
    void postprocess() { }

    void process()
    {
        Info("PythonBuilder process starting");
        this->preprocess();
        this->setup();
        this->configure();
        this->build();
        this->install();
        this->clean();
        this->postprocess();
        Info("PythonBuilder process ending");
    }
};
