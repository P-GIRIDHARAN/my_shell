#include "pipeline.h"
#include "utils.h"
#include "cd.h"
#include "pwd.h"
#include "echo.h"
#include "ls.h"
#include "pinfo.h"
#include "search.h"
#include "background.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <cstring>

using namespace std;

void executeSingleCommandWithRedirection(const vector<string>& args) {
    if (args.empty()) return;

    vector<string> cmdArgs;
    string inputFile, outputFile;
    bool append = false;

    // Parse for I/O redirection symbols
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "<") {
            if (i + 1 < args.size()) inputFile = args[i + 1];
            ++i;
        } else if (args[i] == ">") {
            if (i + 1 < args.size()) { outputFile = args[i + 1]; append = false; }
            ++i;
        } else if (args[i] == ">>") {
            if (i + 1 < args.size()) { outputFile = args[i + 1]; append = true; }
            ++i;
        } else {
            cmdArgs.push_back(args[i]);
        }
    }

    if (cmdArgs.empty()) return;

    string cmd = cmdArgs[0];
    if (cmd == "cd") cdCommand(cmdArgs);
    else if (cmd == "pwd") pwdCommand(cmdArgs);
    else if (cmd == "echo") echoCommand(cmdArgs);
    else if (cmd == "ls") lsCommand(cmdArgs);
    else if (cmd == "pinfo") pinfoCommand(cmdArgs);
    else if (cmd == "search") searchCommand(cmdArgs);
    else {
        vector<char*> argv;
        for (auto &s : cmdArgs) argv.push_back(const_cast<char*>(s.c_str()));
        argv.push_back(nullptr);

        // Fork and execute system command
        pid_t pid = fork();
        if (pid < 0) { perror("fork failed"); exit(1); }
        else if (pid == 0) {
            // Input redirection
            if (!inputFile.empty()) {
                int fd = open(inputFile.c_str(), O_RDONLY);
                if (fd < 0) { perror(("Input file error: " + inputFile).c_str()); exit(1); }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            // Output redirection
            if (!outputFile.empty()) {
                int fd;
                if (append) fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                else fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { perror(("Output file error: " + outputFile).c_str()); exit(1); }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            // Execute
            if (execvp(argv[0], argv.data()) < 0) {
                perror(("Execution failed: " + cmdArgs[0]).c_str());
                exit(1);
            }
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void handlePipeline(const vector<string>& commands) {
    int n = commands.size();
    int prev_fd[2] = {-1, -1};

    for (int i = 0; i < n; ++i) {
        int fd[2];
        if (i < n - 1) {
            if (pipe(fd) < 0) { perror("pipe failed"); return; }
        }

        pid_t pid = fork();
        if (pid < 0) { perror("fork failed"); return; }

        if (pid == 0) {
            // Input from previous pipe
            if (i > 0) {
                dup2(prev_fd[0], STDIN_FILENO);
                close(prev_fd[0]);
                close(prev_fd[1]);
            }
            // Output to next pipe
            if (i < n - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }

            vector<string> args = tokenize(commands[i], " \t");
            executeSingleCommandWithRedirection(args);
            exit(0);
        } else {
            // Close previous pipe in parent
            if (i > 0) {
                close(prev_fd[0]);
                close(prev_fd[1]);
            }
            if (i < n - 1) {
                prev_fd[0] = fd[0];
                prev_fd[1] = fd[1];
            }
        }
    }

    // Wait for all children
    for (int i = 0; i < n; ++i) {
        int status;
        wait(&status);
    }
}
