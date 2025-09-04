#include "cd.h"
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

static string previousDir = "";  

void cdCommand(const vector<string>& args) {
    const char* home = getenv("HOME");
    string targetDir;

    if (args.size() > 2) {
        cerr << "Invalid arguments for cd" << endl;
        return;
    }

    if (args.size() == 1) {
        if (home) targetDir = home;
        else {
            cerr << "HOME not set" << endl;
            return;
        }
    } else {
        string arg = args[1];
        if (arg == "~") {
            if (home) targetDir = home;
            else {
                cerr << "HOME not set" << endl;
                return;
            }
        } else if (arg == "-") {
            if (!previousDir.empty()) targetDir = previousDir;
            else {
                cerr << "OLDPWD not set" << endl;
                return;
            }
        } else {
            targetDir = arg;
        }
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        perror("getcwd");
        return;
    }
    string currentDir(cwd);

    if (chdir(targetDir.c_str()) != 0) {
        perror("cd");
    } else {
        previousDir = currentDir;  
    }
}
