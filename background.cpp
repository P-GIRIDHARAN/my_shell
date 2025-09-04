#include "background.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <cstring>
#include "signals.h"
using namespace std;

void runSystemCommand(const vector<string>& args, bool background) {
    if (args.empty()) return;

    vector<char*> argv;
    for (const string &s : args) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        setpgid(0, 0); 
        if (execvp(argv[0], argv.data()) < 0) {
            perror(("Execution failed: " + args[0]).c_str());
            exit(1);
        }
    } else {
        if (background) {
            cout << pid << endl;
        } else {
            foregroundPid = pid;  
            int status;
            waitpid(pid, &status, WUNTRACED);  
            foregroundPid = -1;  
        }
    }
}
