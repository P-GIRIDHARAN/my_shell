#include "search.h"
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <unistd.h>
using namespace std;

bool searchRecursive(const string& dirPath, const string& target) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;

        if (name == "." || name == "..") continue;

        string fullPath = dirPath + "/" + name;

        if (name == target) {
            closedir(dir);
            return true;
        }

        struct stat st;
        if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            if (searchRecursive(fullPath, target)) {
                closedir(dir);
                return true;
            }
        }
    }

    closedir(dir);
    return false;
}

void searchCommand(const vector<string>& args) {
    if (args.size() != 2) {
        cerr << "search: Invalid arguments" << endl;
        return;
    }

    string target = args[1];
    char cwd[1024];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }

    bool found = searchRecursive(string(cwd), target);

    if (found) cout << "True" << endl;
    else cout << "False" << endl;
}
