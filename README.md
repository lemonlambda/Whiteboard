# Whiteboard is a simple buildtool for C
Whiteboard is a spmile buildtool for C.

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
