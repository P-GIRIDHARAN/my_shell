#include "pinfo.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <signal.h>
#include <cstdio>

using namespace std;

void pinfoCommand(const vector<string>& args) {
    pid_t pid;

    if (args.size() == 1) {
        pid = getpid();
    } else {
        try {
            pid = stoi(args[1]);
        } catch (...) {
            cerr << "pinfo: Invalid PID" << endl;
            return;
        }
    }

    string procStatPath = "/proc/" + to_string(pid) + "/stat";
    string procExePath = "/proc/" + to_string(pid) + "/exe";

    FILE* statFile = fopen(procStatPath.c_str(), "r");
    if (!statFile) {
        cerr << "pinfo: Unable to open " << procStatPath << endl;
        return;
    }

    char buffer[4096];
    if (!fgets(buffer, sizeof(buffer), statFile)) {
        cerr << "pinfo: Failed to read " << procStatPath << endl;
        fclose(statFile);
        return;
    }
    fclose(statFile);

    stringstream ss(buffer);
    string pidStr, comm, state;
    ss >> pidStr >> comm >> state;

    string dummy;
    for (int i = 0; i < 17; ++i) ss >> dummy;
    unsigned long vsize;
    ss >> vsize;

    pid_t fg_pgrp = tcgetpgrp(STDIN_FILENO);
    pid_t pgid = getpgid(pid);
    if (pgid == fg_pgrp && (state == "R" || state == "S"))
        state += "+";


    string exePath;
    char buf[1024];
    ssize_t len = readlink(procExePath.c_str(), buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        exePath = replaceHomeDir(string(buf));
    } else {
        exePath = "Executable path not found";
    }

    cout << "pid -- " << pidStr << endl;
    cout << "Process Status -- " << state << endl;
    cout << "memory -- " << vsize << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << exePath << endl;
}
