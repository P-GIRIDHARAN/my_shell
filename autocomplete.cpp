#include "autocomplete.h"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdlib>
#include <algorithm>
#include <cstring>

using std::cout;
using std::endl;

vector<string> shellCommands = {"cd", "pwd", "echo", "ls", "pinfo", "search", "history", "exit"};

bool isExecutable(const string &path) {
    struct stat sb;
    return stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IXUSR;
}

vector<string> getExecutablesInPath(const string &prefix) {
    vector<string> matches;
    const char *pathEnv = getenv("PATH");
    if (!pathEnv) return matches;

    string pathStr(pathEnv);
    size_t start = 0, end;

    while ((end = pathStr.find(':', start)) != string::npos) {
        string dir = pathStr.substr(start, end - start);
        start = end + 1;

        DIR *dp = opendir(dir.c_str());
        if (!dp) continue;
        struct dirent *entry;
        while ((entry = readdir(dp))) {
            string name(entry->d_name);
            if (name.find(prefix) == 0 && isExecutable(dir + "/" + name))
                matches.push_back(name);
        }
        closedir(dp);
    }

    string dir = pathStr.substr(start);
    DIR *dp = opendir(dir.c_str());
    if (dp) {
        struct dirent *entry;
        while ((entry = readdir(dp))) {
            string name(entry->d_name);
            if (name.find(prefix) == 0 && isExecutable(dir + "/" + name))
                matches.push_back(name);
        }
        closedir(dp);
    }

    return matches;
}



char* command_generator(const char *text, int state) {
    static vector<string> matches;
    static size_t index;

    if (state == 0) {
        matches.clear();
        index = 0;
        string prefix(text);

        for (auto &cmd : shellCommands)
            if (cmd.find(prefix) == 0) matches.push_back(cmd);

        vector<string> pathCmds = getExecutablesInPath(prefix);
        matches.insert(matches.end(), pathCmds.begin(), pathCmds.end());

        
        std::sort(matches.begin(), matches.end());
        matches.erase(std::unique(matches.begin(), matches.end()), matches.end());
    }

    if (index < matches.size()) return strdup(matches[index++].c_str());
    return nullptr;
}

char** custom_completion(const char *text, int start, int end) 
{
    if (start>0)
    {
        rl_attempted_completion_over = 0;
        return nullptr;
    }

    return rl_completion_matches(text, command_generator);
}

void display_matches_hook(char **matches, int count, int max_length) {
    if (!matches || count == 0) return;

    cout << endl;
    for (int i = 0; i < count; ++i)
        cout << matches[i] << "  ";
    cout << endl;

    rl_on_new_line();
    rl_redisplay();
}

void setupAutocomplete() {
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_attempted_completion_function = custom_completion;

}
