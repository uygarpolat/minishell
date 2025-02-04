# Minishell

Welcome to Minishell, a custom-built shell inspired by Bash, developed entirely in C by Uygar Polat ([@uygarpolat](https://github.com/uygarpolat/)) and Helmi Pirkola ([@hpirkola](https://github.com/hpirkola/)) as part of [Hive Helsinki](https://www.hive.fi/en/) (42) curriculum.

## What's in the Box?

Minishell packs all the core features you'd expect from a modern shell, including:

- **Interactive Prompt**: Our shell gives you a familiar command prompt with support for command history, so you can easily cycle through your previous inputs.
- **Command Execution**: Run external programs or use built-in commands like `echo`, `cd`, `pwd`, `export`, `unset`, `env`, and `exit`. Minishell seamlessly handles these, making process management feel effortless.
- **Redirection & Piping**: Need to redirect output or chain commands together? We've got you covered with full support for input/output redirection (`<`, `>`, `>>`, `<<`) and the pipe (`|`) operator.
- **Environment Variables**: Use `$VARIABLE` to access environment data or `$?` to check the last command's exit status, just like you would in Bash.
- **Wildcard Expansion**: Simplify file handling with `*` to match patterns in the current directory.
- **Signal Handling**: Minishell gracefully handles signals like `Ctrl+C` (to interrupt), `Ctrl+D` (to exit), and `Ctrl+\` (to ignore in interactive mode).

## How to Get Started

Clone the repo and build Minishell using the included `Makefile`:

```bash
git clone https://github.com/uygarpolat/minishell.git
cd minishell
make
```

Once compiled, launch it with:

```
./minishell
```
