*This project has been created as part of the 42 curriculum by ebansse and cguinot.*

# minishell

A simplified Unix shell written in C, modelled on **bash**. It reads a command line, splits it into tokens, expands variables, sets up pipes and redirections, then runs builtins or external programs in child processes.

## Features

**Prompt and line editing**
- Interactive prompt with history (GNU Readline), so you can recall previous commands with the arrow keys

**Parsing**
- Single quotes `'...'`: nothing inside is interpreted
- Double quotes `"..."`: only `$` is expanded
- Environment variables `$VAR` and the last exit status `$?`
- Syntax errors (unclosed quotes, `| |`, a redirection with no file name) are reported instead of executed

**Execution**
- External commands found through `$PATH`, or given as a relative or absolute path
- Pipes `cmd1 | cmd2 | ... | cmdN`
- Redirections: `<` input, `>` output (truncate), `>>` append, `<<` here-document
- Exit status of the last command stored in `$?` (`127` for a command that isn't found, `130` after `Ctrl-C`, …)

**Builtins**

| Builtin | Behaviour |
|---|---|
| `echo [-n]` | print arguments |
| `cd [path]` | change directory and update `PWD` / `OLDPWD` |
| `pwd` | print the current directory |
| `export [NAME=value]` | set variables, or list them when called without arguments |
| `unset NAME` | remove variables |
| `env` | print the environment |
| `exit [n]` | quit with status `n` |

**Signals** (same behaviour as bash)
- `Ctrl-C` shows a new prompt on a new line
- `Ctrl-D` exits the shell
- `Ctrl-\` does nothing at the prompt

## Build & run

Requires the Readline development headers (`libreadline-dev` on Debian/Ubuntu).

```bash
make
./minishell
```

```
minishell $ export NAME=42
minishell $ echo "Hello $NAME" | tr a-z A-Z > out.txt
minishell $ cat < out.txt
HELLO 42
minishell $ cat << EOF
> first line
> second line
> EOF
first line
second line
minishell $ exit
```

## How it works

1. **Lexer** (`parse/lexer.c`): splits the line into words and operators (`|`, `<`, `>`, `<<`, `>>`), respecting quotes
2. **Expansion** (`parse/var.c`): replaces `$VAR` and `$?`, then removes the quotes
3. **Parser** (`parse/parser.c`): builds a list of commands, each with its arguments and redirections
4. **Executor** (`exe/`): creates the pipes, forks one process per command, applies the redirections, and runs a builtin in the parent when it is a single command (so `cd`, `export` and `exit` affect the shell itself)

## Project structure

| Path | Role |
|---|---|
| `main.c` | read-eval loop |
| `parse/` | lexer, variable expansion, parser |
| `exe/` | pipelines, redirections, builtin dispatch, external commands |
| `builtins/` | one file per builtin |
| `env/` | environment copy and helpers |
| `others/` | signals, memory cleanup, utilities |
| `libft/` | my Libft |
