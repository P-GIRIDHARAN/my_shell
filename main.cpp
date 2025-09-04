#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>

#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "ls.h"
#include "pinfo.h"
#include "search.h"
#include "io_redirection.h"
#include "pipeline.h"
#include "background.h"
#include "signals.h"
#include "autocomplete.h"
#include "history.h"
#include "utils.h"

string SHELL_HOME;

using namespace std;

string getUsername() {
    struct passwd* pw = getpwuid(getuid());
    return string(pw->pw_name);
}

string getHostName() {
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    return string(hostname);
}

string getCurrentDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    string dir(cwd);

    if (!SHELL_HOME.empty() && dir.find(SHELL_HOME) == 0) {
        if (dir == SHELL_HOME) {
            dir = "~";  
        } else {
            dir.replace(0, SHELL_HOME.size(), "~");
        }
    }
    return dir;
}




void executeCommand(string cmd) {
    cmd = trim(cmd);
    if (cmd.empty()) return;

    bool background = false;
    if (cmd.back() == '&') {
        background = true;
        cmd.pop_back();
        cmd = trim(cmd);
    }
    if (cmd.find('|') != string::npos) {
        vector<string> pipeCmds = tokenize(cmd, "|");
        handlePipeline(pipeCmds);
        return;
    }

    vector<string> args = tokenize(cmd, " \t");
    if (args.empty()) return;

    for (const auto& s : args) {
        if (s == "<" || s == ">" || s == ">>") {
            handleIORedirection(args);
            return;
        }
    }

    string command = args[0];
    if (command == "exit") {
        exit(0);
    } else if (command == "cd") {
        cdCommand(args);
    } else if (command == "pwd") {
        pwdCommand(args);
    } else if (command == "echo") {
        echoCommand(args);
    } else if (command == "ls") {
        lsCommand(args);
    } else if (command == "pinfo") {
        pinfoCommand(args);
    } else if (command == "search") {
        searchCommand(args);
    } else if (command == "history") {
        historyCommand(args);
    } else {
        runSystemCommand(args, background);
    }

    addToHistory(cmd);  
}

int main() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    SHELL_HOME = string(cwd);

    setupSignalHandlers();
    atexit(saveHistory);

    loadHistory();  

    string username = getUsername();
    string hostname = getHostName();

    string historyFile = string(getenv("HOME")) + "/history.txt";
    read_history(historyFile.c_str());

    while (true) {
        string prompt = username + "@" + hostname + ":" + getCurrentDir() + "> ";
        char* line = readline(prompt.c_str());
        if (!line) {
            cout << endl; 
            break;
        }

        string input(line);
        free(line);

        if (!input.empty()) {
            add_history(input.c_str());   
            addToHistory(input);         
        }

        vector<string> commands = tokenize(input, ";");
        for (auto &cmd : commands) {
            executeCommand(cmd);
        }
    }

    write_history(historyFile.c_str());

    return 0;
}
