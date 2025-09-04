#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>

// List of built-in shell commands
std::vector<std::string> shellCommands = {
    "cd", "pwd", "echo", "ls", "pinfo", "search", "history", "exit"
};

// Helper: Check if file is executable
bool isExecutable(const std::string& path) {
    struct stat sb;
    return (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IXUSR);
}

// Get all executable commands in $PATH
std::vector<std::string> getExecutablesInPath(const std::string& prefix) {
    std::vector<std::string> matches;
    const char* pathEnv = getenv("PATH");
    if (!pathEnv) return matches;

    std::string pathStr(pathEnv);
    size_t start = 0, end;

    while ((end = pathStr.find(':', start)) != std::string::npos) {
        std::string dir = pathStr.substr(start, end - start);
        start = end + 1;

        DIR* dp = opendir(dir.c_str());
        if (!dp) continue;

        struct dirent* entry;
        while ((entry = readdir(dp)) != nullptr) {
            std::string name(entry->d_name);
            if (name.find(prefix) == 0) {
                std::string fullPath = dir + "/" + name;
                if (isExecutable(fullPath)) matches.push_back(name);
            }
        }
        closedir(dp);
    }

    // Last segment
    std::string dir = pathStr.substr(start);
    DIR* dp = opendir(dir.c_str());
    if (dp) {
        struct dirent* entry;
        while ((entry = readdir(dp)) != nullptr) {
            std::string name(entry->d_name);
            if (name.find(prefix) == 0) {
                std::string fullPath = dir + "/" + name;
                if (isExecutable(fullPath)) matches.push_back(name);
            }
        }
        closedir(dp);
    }

    return matches;
}

// Get files in current directory matching prefix
std::vector<std::string> getFilesInCWD(const std::string& prefix) {
    std::vector<std::string> matches;
    DIR* dir = opendir(".");
    if (!dir) return matches;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);
        if (name.find(prefix) == 0) matches.push_back(name);
    }
    closedir(dir);
    return matches;
}

// Completion generator
char* command_generator(const char* text, int state) {
    static std::vector<std::string> matches;
    static size_t match_index;

    if (state == 0) {
        matches.clear();
        match_index = 0;

        std::string prefix(text);

        // Add built-in shell commands
        for (auto& cmd : shellCommands) {
            if (cmd.find(prefix) == 0) matches.push_back(cmd);
        }

        // Add executables in PATH
        std::vector<std::string> pathCmds = getExecutablesInPath(prefix);
        matches.insert(matches.end(), pathCmds.begin(), pathCmds.end());

        // Add files/directories in current dir
        std::vector<std::string> files = getFilesInCWD(prefix);
        matches.insert(matches.end(), files.begin(), files.end());

        // Remove duplicates
        std::sort(matches.begin(), matches.end());
        matches.erase(std::unique(matches.begin(), matches.end()), matches.end());
    }

    if (match_index < matches.size()) {
        return strdup(matches[match_index++].c_str());
    }

    return nullptr;
}

// Readline completion function
char** custom_completion(const char* text, int start, int end) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, command_generator);
}

// Setup autocomplete
void setupAutocomplete() {
    rl_attempted_completion_function = custom_completion;
}
