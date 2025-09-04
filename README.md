# Custom Linux Shell - Assignment 2

This project is a custom shell implemented in C++ for the AOS course assignment. It supports basic shell features such as command execution, piping, I/O redirection, background jobs, command history, tab autocompletion, and several built-in commands.

## Features

- **Built-in Commands:** `cd`, `pwd`, `echo`, `ls`, `pinfo`, `search`, `history`, `exit`
- **External Commands:** Supports running any executable available in `$PATH`
- **Piping:** Supports chaining commands using `|`
- **I/O Redirection:** Supports `<`, `>`, and `>>` for input/output redirection
- **Background Execution:** Use `&` to run processes in the background
- **Command History:** Maintains a persistent history of commands
- **Tab Autocompletion:** For commands and executables
- **Signal Handling:** Handles `Ctrl+C` and `Ctrl+Z` gracefully

## File Structure

| File                | Purpose                                                                 |
|---------------------|-------------------------------------------------------------------------|
| `main.cpp`          | Main shell loop, command parsing, and dispatching                       |
| `cd.cpp`            | Implementation of the `cd` command                                      |
| `pwd.cpp`           | Implementation of the `pwd` command                                     |
| `echo.cpp`          | Implementation of the `echo` command                                    |
| `ls.cpp`            | Implementation of the `ls` command                                      |
| `pinfo.cpp`         | Implementation of the `pinfo` command                                   |
| `search.cpp`        | Implementation of the `search` command                                  |
| `history.cpp`       | Command history management (load, save, print, add)                     |
| `io_redirection.cpp`| Handles input/output redirection for commands                           |
| `pipeline.cpp`      | Handles command pipelines (`|`)                                         |
| `background.cpp`    | Handles execution of background processes                               |
| `signals.cpp`       | Signal handler setup and logic                                          |
| `autocomplete.cpp`  | Tab completion for commands and executables                             |
| `utils.cpp`         | Utility functions (tokenization, trimming, etc.)                        |
| `*files.h`               | Header files for each module 