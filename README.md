# Whiteboard is a simple buildtool for C
Whiteboard is a simple buildtool for C, meant to be easy to use.

# Features
* Designed for simplicity
* Uses TOML for configuration
* Supports testing frameworks
* Allows easy change of compiler and linker programs and flags
* Use of custom source extensions

# How it works
Whiteboard will create a target directory in your project root, and inside that a directory for each package, with the build artifacts, like so:
```
target/
  YourProjectName/
    bin/
    obj/
```

# Compiling
You can run these commands with gcc to compile (on linux) or figure it out on your own.
```
mkdir -p target/Whiteboard && mkdir -p target/Whiteboard/obj target/Whiteboard/bin
gcc -O2 -Wall -Wextra -c $(find src -type f -name "*.c") -I src
mv *.o target/Whiteboard/obj
gcc -B gcc target/Whiteboard/obj/* -o target/Whiteboard/bin/NewRelease-0.0.1.0
```
Windows compilation (might be out of date so you might need to add some new C files to the command)
```
mkdir target && mkdir target\Whiteboard && mkdir target\Whiteboard\obj && mkdir target\Whiteboard\bin
gcc -O2 -Wall -Wextra -c src\args_parser.c src\main.c src\run_bin.c src\stages.c src\string_replace.c src\toml.c src\toml_format.c src\vector.c -I src
mv *.o target\Whiteboard\obj
gcc -B gcc target\Whiteboard\obj\* -o target\Whiteboard\bin\NewRelease-0.0.1.0
```

# CLI Usage
`whiteboard run <name?>` If no name is supplied builds and runs the default bin otherwise it will build and run the bin with that name.

`whiteboard build <name?>` If no name is supplied it builds the default bin otherwise it will build the bin with that name.

`whiteboard test <name>` Builds and runs the test with that name.

`whiteboard clean` Removes the target dir.


# Examples
This is an example config you can use (and it will use gcc by default)
```toml
[package]
name = "Example"
version = "0.1.0.0"

[[bin]]
default = true
name = "Example"
```
Now you can run `whiteboard run` to run the project, where whiteboard is the exe of whiteboard.

Another example is
```toml
[package]
name = "Example2"
version = "0.1.0.0"

[[bin]]
default = true
name = "Example2"
cc = "clang"
ld = "clang"
ccargs = "-O3 -Wall -Wextra"
```
You can see this uses a different compiler and linker from the default `gcc` and adds some extra args.

Here is a completely filled out toml
```toml
[package]
name = "Example3"
version = "0.1.0.0"

[[bin]]
default = true
name = "Example3"
srcdir = ["src/"] # can either be an array or a string
includedir = "src/include" # only a string
cc = "clang"
ld = "clang"
ccargs = "-O3"
ldargs = "-B clang"
extension = "c" # Extension of the source files

[[test]]
name = "Test"
srcdir = ["test/Test"] # can either be an array or a string
includedir = "test/Test/include" # only a string
programincludedir = "src/include" # where to get the include dir from the actual code
cc = "clang"
ld = "clang"
ccargs = "-O3"
ldargs = "-B clang"
extension = "c" # Extension of the source files
```

# The heart of this project are our contributors 💖 : 

<a href="https://github.com/LemonjamesD/Whiteboard/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=LemonjamesD/Whiteboard"/>
</a>
