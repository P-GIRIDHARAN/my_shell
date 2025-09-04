#include "autocomplete.h"
#include <dirent.h>
#include <algorithm>

using namespace std;

vector<string> splitWords(const string &input) {
    vector<string> words;
    size_t pos = 0, start = 0;
    while ((pos = input.find(' ', start)) != string::npos) {
        if (pos != start)
            words.push_back(input.substr(start, pos - start));
        start = pos + 1;
    }
    if (start < input.size())
        words.push_back(input.substr(start));
    return words;
}

vector<string> getInternalCommands() {
    return {"cd", "echo", "pwd", "ls", "pinfo", "search", "history", "exit"};
}

vector<string> getFiles(const string &prefix) {
    vector<string> matches;
    DIR* dp = opendir(".");
    if (!dp) return matches;
    struct dirent* entry;
    while ((entry = readdir(dp)) != nullptr) {
        string name(entry->d_name);
        if (name.find(prefix) == 0)
            matches.push_back(name);
    }
    closedir(dp);
    sort(matches.begin(), matches.end());
    return matches;
}

string commonPrefix(const vector<string> &words) {
    if (words.empty()) return "";
    string prefix = words[0];
    for (size_t i = 1; i < words.size(); ++i) {
        size_t j = 0;
        while (j < prefix.size() && j < words[i].size() && prefix[j] == words[i][j])
            j++;
        prefix = prefix.substr(0, j);
    }
    return prefix;
}
