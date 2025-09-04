# Custom Shell - Assignment 2

This project is a custom shell implemented in C++. It supports basic shell features such as command execution, piping, I/O redirection, background jobs, command history, tab autocompletion, and several built-in commands.


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
| `*.h`               | Header files for each module 


## How to Build

To build the shell, simply run:

```sh
make
```

This will compile all source files and produce the executable `a.out`.

## How to Run

To start the shell, run:

```sh
./a.out
```


## Supported Commands

---

##### 1. `cd <directory>`
**Input:** `cd directory`  
**Output:** Navigated to `/absolute/path/to/directory`  
Moves into the specified directory. Supports `.`, `..`, `-`, and `~`. Prints an error for invalid arguments.

---

##### 2. `cd ..`
**Input:** `cd ..`  
**Output:** Moves one level up in the directory hierarchy.

---

##### 3. `cd ~`
**Input:** `cd ~`  
**Output:** Moves to the user's home directory.

---

##### 4. `cd -`
**Input:** `cd -`  
**Output:** Switches to the previous working directory.

---

##### 5. `pwd`
**Input:** `pwd`  
**Output:** Prints the absolute path of the current working directory.

---

##### 6. `echo <text>`
**Input:** `echo "hello" ""world"`  
**Output:** `"hello" ""world"`  
Prints the arguments preserving quoted segments.

---

##### 7. `ls`
**Input:** `ls`  
**Output:** Lists files and directories in the current folder.

---

##### 8. `ls -a`
**Input:** `ls -a`  
**Output:** Lists all files including hidden files.

---

##### 9. `ls -l`
**Input:** `ls -l`  
**Output:** Detailed listing of files including permissions, owner, size, and modification date.

---

##### 10. `ls -al` or `ls -la`
**Input:** `ls -al`  
**Output:** Combined output of `-a` and `-l`.

---

##### 11. `ls <directory>`
 
**Input:** `ls test_dir`  
**Output:** Lists files inside the specified directory.

---

##### 12. `ls -<flags> <directory>`
  
**Input:** `ls -al test_dir`  
**Output:** Detailed listing with flags applied to the specified directory.

---

##### 13. `<system_command>`
  
**Input:** `gedit`  
**Output:** Opens gedit in foreground. Shell waits until completion.

---

##### 14. `<system_command> &`
  
**Input:** `gedit &`  
**Output:** Launches gedit in background and prints PID.

---

##### 15. `pinfo`
  
**Input:** `pinfo`  
**Output:** Displays shell process information including status, memory, and executable path.

---

##### 16. `pinfo <pid>`
  
**Input:** `pinfo 1234`  
**Output:** Displays status, memory, and executable path of specified PID.

---

##### 17. `search <filename>`
  
**Input:** `search xyz.txt`  
**Output:** `True` or `False` depending on whether the file exists.

---

##### 18. Input redirection `<`
  
**Input:** `cat < input.txt`  
**Output:** Reads from input.txt and prints to stdout.

---

##### 19. Output redirection `>`
  
**Input:** `echo hello > output.txt`  
**Output:** Writes "hello" to output.txt (overwrites if exists).

---

##### 20. Append output `>>`
  
**Input:** `echo hello >> output.txt`  
**Output:** Appends "hello" to output.txt.

---

##### 21. Single pipe `|`
  
**Input:** `cat file.txt | wc`  
**Output:** Sends output of `cat` to `wc`.

---

##### 22. Multiple pipes
  
**Input:** `cat sample.txt | head -7 | tail -5`  
**Output:** Supports multiple commands piped together.

---

##### 23. Redirection with pipe
  
**Input:** `ls | grep ".txt" > out.txt`  
**Output:** Sends matching output to `out.txt`.

---

##### 24. History
  
**Input:** `history` or `history <num>`  
**Output:** Displays last 10 commands (or `<num>` commands). Supports UP/DOWN arrow navigation.
