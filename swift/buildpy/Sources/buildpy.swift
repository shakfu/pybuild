import ArgumentParser

@main
struct buildpy: ParsableCommand {
    @Option(help: "python version") 
    var version: String = "3.11.7"

    @Option(help: "name of build config")
    var config: String = "static.max"
    
    @Option(help: "configure options")
    var opts: [String] = []  
    
    @Option(help: "python packages to install")
    var pkgs: [String] = []
    
    @Flag(help: "build debug python")
    var debug = false
    
    @Flag(help: "optimize build")
    var optimize = false
    
    @Flag(help: "reset build")
    var reset = false

    mutating func run() throws {
        print("""
            version: \(version)
            config: \(config)
            opts: \(opts)
            pkgs: \(pkgs)
            debug: \(debug)
            optimize: \(optimize)
            reset: \(reset)
            """
        )

        let res = execute(exe: "/usr/bin/git", args: "--version")
        print("output: \(res.out)")
        print("error: \(res.err)")
    }
}