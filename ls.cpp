#include "ls.h"
#include "utils.h"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

string getPermissions(mode_t mode) {
    string perm = "----------";
    if (S_ISDIR(mode)) perm[0] = 'd';
    if (mode & S_IRUSR) perm[1] = 'r';
    if (mode & S_IWUSR) perm[2] = 'w';
    if (mode & S_IXUSR) perm[3] = 'x';
    if (mode & S_IRGRP) perm[4] = 'r';
    if (mode & S_IWGRP) perm[5] = 'w';
    if (mode & S_IXGRP) perm[6] = 'x';
    if (mode & S_IROTH) perm[7] = 'r';
    if (mode & S_IWOTH) perm[8] = 'w';
    if (mode & S_IXOTH) perm[9] = 'x';
    return perm;
}

void printEntry(const string &path, const string &name, bool longFormat) {
    if (!longFormat) {
        cout << name << "  ";
        return;
    }
    struct stat st;
    string fullPath = path + "/" + name;
    if (stat(fullPath.c_str(), &st) != 0) { perror(("stat: " + fullPath).c_str()); return; }
    string perms = getPermissions(st.st_mode);
    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
    cout << perms << " "
         << st.st_nlink << " "
         << (pw ? pw->pw_name : "unknown") << " "
         << (gr ? gr->gr_name : "unknown") << " "
         << st.st_size << " "
         << timebuf << " "
         << name << endl;
}

void printDirectory(const string &path, bool showAll, bool longFormat) {
    DIR *dir = opendir(path.c_str());
    if (!dir) { perror(("ls: " + path).c_str()); return; }

    vector<string> files;
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if (!showAll && name[0] == '.') continue;
        files.push_back(name);
    }
    sort(files.begin(), files.end());
    for (const string &file : files) printEntry(path, file, longFormat);
    if (!longFormat) cout << endl;
    closedir(dir);
}

void lsCommand(const vector<string>& args) {
    bool showAll = false;
    bool longFormat = false;
    vector<string> targets;

    if (args.size() == 1) targets.push_back(".");
    else {
        for (size_t i = 1; i < args.size(); ++i) {
            string arg = args[i];
            if (!arg.empty() && arg[0] == '-') {
                for (size_t j = 1; j < arg.size(); ++j) {
                    if (arg[j] == 'a') showAll = true;
                    else if (arg[j] == 'l') longFormat = true;
                }
            } else {
                if (arg == "~") arg = getenv("HOME") ? getenv("HOME") : ".";
                targets.push_back(arg);
            }
        }
    }

    if (targets.empty()) targets.push_back(".");

    for (size_t i = 0; i < targets.size(); ++i) {
        struct stat st;
        if (stat(targets[i].c_str(), &st) != 0) { perror(("ls: " + targets[i]).c_str()); continue; }

        if (S_ISDIR(st.st_mode)) {
            if (targets.size() > 1) cout << targets[i] << ":" << endl;
            printDirectory(targets[i], showAll, longFormat);
            if (i != targets.size() - 1) cout << endl;
        } else {
            printEntry(".", targets[i], longFormat);
        }
    }
}
