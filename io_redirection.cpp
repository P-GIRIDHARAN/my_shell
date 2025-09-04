#include "io_redirection.h"
#include "background.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>

using namespace std;

void handleIORedirection(const vector<string>& args) {
    if (args.empty()) return;

    vector<string> cmdArgs;
    string inputFile;
    string outputFile;
    bool append = false;

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "<") {
            if (i + 1 >= args.size()) {
                cerr << "Error: no input file specified" << endl;
                return;
            }
            inputFile = args[i + 1];
            ++i;
        } else if (args[i] == ">") {
            if (i + 1 >= args.size()) {
                cerr << "Error: no output file specified" << endl;
                return;
            }
            outputFile = args[i + 1];
            append = false;
            ++i;
        } else if (args[i] == ">>") {
            if (i + 1 >= args.size()) {
                cerr << "Error: no output file specified" << endl;
                return;
            }
            outputFile = args[i + 1];
            append = true;
            ++i;
        } else {
            cmdArgs.push_back(args[i]);
        }
    }

    if (cmdArgs.empty()) return;

    vector<char*> argv;
    for (const string& s : cmdArgs) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        if (!inputFile.empty()) {
            int fd = open(inputFile.c_str(), O_RDONLY);
            if (fd < 0) {
                perror(("Input file error: " + inputFile).c_str());
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (!outputFile.empty()) {
            int fd;
            if (append) fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            else fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd < 0) {
                perror(("Output file error: " + outputFile).c_str());
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        if (execvp(argv[0], argv.data()) < 0) {
            perror(("Execution failed: " + cmdArgs[0]).c_str());
            exit(1);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}
