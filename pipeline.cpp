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

void executeCommandWithRedirection(const vector<string>& args, const string& inputFile, const string& outputFile, bool append) {
    if (args.empty()) return;

    string cmd = args[0];
    if (cmd == "cd") { cdCommand(args); return; }
    if (cmd == "pwd") { pwdCommand(args); return; }
    if (cmd == "echo") { echoCommand(args); return; }
    if (cmd == "ls") { lsCommand(args); return; }
    if (cmd == "pinfo") { pinfoCommand(args); return; }
    if (cmd == "search") { searchCommand(args); return; }

    vector<char*> argv;
    for (auto &s : args) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) { perror("fork failed"); return; }
    if (pid == 0) {
        setpgid(0,0);
        if (!inputFile.empty()) {
            int fd = open(inputFile.c_str(), O_RDONLY);
            if (fd < 0) { perror(("Input file: " + inputFile).c_str()); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (!outputFile.empty()) {
            int fd = append ? open(outputFile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644) : open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror(("Output file: " + outputFile).c_str()); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(argv[0], argv.data());
        perror(("Execution failed: " + cmd).c_str());
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, WUNTRACED);
    }
}

void handlePipeline(const vector<string>& commands, bool background) {
    int n = commands.size();
    int prev_fd[2] = {-1, -1};

    for (int i = 0; i < n; ++i) {
        string cmd = trim(commands[i]);
        vector<string> args = tokenize(cmd, " \t", true);

        string inputFile, outputFile;
        bool append = false;

        if (i == 0) {
            for (size_t j = 0; j < args.size(); ++j) {
                if (args[j] == "<" && j+1 < args.size()) {
                    inputFile = args[j+1];
                    args.erase(args.begin()+j, args.begin()+j+2);
                    --j;
                }
            }
        }

        if (i == n-1) {
            for (size_t j = 0; j < args.size(); ++j) {
                if ((args[j] == ">" || args[j] == ">>") && j+1 < args.size()) {
                    outputFile = args[j+1];
                    append = (args[j] == ">>");
                    args.erase(args.begin()+j, args.begin()+j+2);
                    --j;
                }
            }
        }

        int fd[2] = {-1, -1};
        if (i < n - 1 && pipe(fd) < 0) { perror("pipe"); return; }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return; }

        if (pid == 0) { // child
            setpgid(0, 0);

            if (i > 0) { dup2(prev_fd[0], STDIN_FILENO); close(prev_fd[0]); close(prev_fd[1]); }
            if (i < n - 1) { dup2(fd[1], STDOUT_FILENO); close(fd[0]); close(fd[1]); }

            if (i == 0 && !inputFile.empty()) {
                int fdin = open(inputFile.c_str(), O_RDONLY);
                if (fdin < 0) { perror(("Input: "+inputFile).c_str()); exit(1); }
                dup2(fdin, STDIN_FILENO);
                close(fdin);
            }

            if (i == n-1 && !outputFile.empty()) {
                int fdout = append ? open(outputFile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644)
                                   : open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fdout < 0) { perror(("Output: "+outputFile).c_str()); exit(1); }
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
            }

            vector<char*> argv;
            for (auto &s : args) argv.push_back(const_cast<char*>(s.c_str()));
            argv.push_back(nullptr);
            execvp(argv[0], argv.data());
            perror(("Execution failed: " + args[0]).c_str());
            exit(1);
        } else { // parent
            if (i > 0) { close(prev_fd[0]); close(prev_fd[1]); }
            if (i < n - 1) { prev_fd[0] = fd[0]; prev_fd[1] = fd[1]; }
        }
    }

    if (!background) {
        for (int i = 0; i < n; ++i) {
            int status;
            wait(&status);
        }
    }
}
