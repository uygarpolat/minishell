# Minishell

Welcome to Minishell, a custom-built shell inspired by Bash, developed entirely in C by [Uygar Polat](https://github.com/uygarpolat/) and [Helmi Pirkola](https://github.com/hpirkola/) as part of [Hive Helsinki](https://www.hive.fi/en/) (42) curriculum.

## What's in the Box?

Minishell packs all the core features you'd expect from a modern shell, including:

- **Interactive Prompt**: Our shell gives you a familiar command prompt with support for command history, so you can easily cycle through your previous inputs.
- **Command Execution**: Run external programs or use built-in commands like `echo`, `cd`, `pwd`, `export`, `unset`, `env`, and `exit`. Minishell seamlessly handles these, making process management feel effortless.
- **Redirection & Piping**: Need to redirect output or chain commands together? We've got you covered with full support for input/output redirection (`<`, `>`, `>>`, `<<`) and the pipe (`|`) operator.
- **Environment Variables**: Use `$VARIABLE` to access environment data or `$?` to check the last command's exit status, just like you would in Bash.
- **Wildcard Expansion**: Simplify file handling with `*` to match patterns in the current directory.
- **Signal Handling**: Minishell gracefully handles signals like `Ctrl+C` (to interrupt), `Ctrl+D` (to exit), and `Ctrl+\` (to ignore in interactive mode).

## How to run

1. Clone the repo and move into the project directory:

- `git clone https://github.com/uygarpolat/minishell.git && cd minishell`

2. To get started, make sure [Docker](https://www.docker.com/get-started) is installed on your system. If you don't have Docker or rather not use it, you can also compile the code directly on your machine via `make` command. If so, you can skip to step 4 after simply running `make` and then `./minishell`. Otherwise, build the Docker image with:

- `docker build -t minishell .`
 
3. Run the Minishell via:

- `docker run -it --rm minishell`

4. This will drop you directly into the Minishell prompt and you are ready to use it. Go crazy testing/using it! You can type `exit`, or press Ctrl+D, any time to quit.
