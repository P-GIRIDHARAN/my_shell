#include "utils.h"
#include <algorithm>
#include <sstream>
#include <cstring>
using namespace std;

string trim(const string &str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

vector<string> tokenize(const string &str, const string &delim = " \t") {
    vector<string> tokens;
    string token = "";
    bool inQuotes = false;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];

        if (c == '"') {
            inQuotes = !inQuotes;
            continue; 
        }

        if (!inQuotes && delim.find(c) != string::npos) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c; 
        }
    }

    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

string replaceHomeDir(const string &path) {
    const char *home = getenv("HOME");
    string result = path;
    if (home && result.find(home) == 0) {
        result.replace(0, strlen(home), "~");
    }
    return result;
}
