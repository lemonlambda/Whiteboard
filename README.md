# Whiteboard is a simple buildtool for C
Whiteboard is a spmile buildtool for C.

# Compiling
You can run these commands with gcc to compile (on linux) or figure it out on your own.
```
mkdir -p target/Whiteboard && mkdir -p target/Whiteboard/obj target/Whiteboard/bin
gcc -O2 -Wall -Wextra -c $(find src/* | grep *.c) -I src
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
