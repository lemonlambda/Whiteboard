# Whiteboard is a simple buildtool for C
Whiteboard is a spmile buildtool for C.

# Compiling
You can run these commands with gcc to compile or figure it out on your own.
```
mkdir -p target/Whiteboard && mkdir -p target/Whiteboard/obj target/Whiteboard/bin
gcc -O2 -Wall -Wextra -c src/args_parser.c src/main.c src/run_bin.c src/stages.c src/string_replace.c src/toml.c src/toml_format.c src/vector.c -I src
mv *.o target/Whiteboard/obj
gcc -B gcc target/Whiteboard/obj/* -o target/Whiteboard/bin/NewRelease-0.0.1.0
```

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
