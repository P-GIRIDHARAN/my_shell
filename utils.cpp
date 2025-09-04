#include "utils.h"
#include <cctype>
#include <cstdlib>
#include <cstring>

using namespace std;

string trim(const string &str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}
vector<string> tokenize(const string &str,
                        const string &delims,
                        bool preserveQuotes) {
    vector<string> result;
    string token;
    bool inQuotes = false;
    char quoteChar = 0;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (preserveQuotes && (c == '"' || c == '\'')) {
            if (inQuotes && c == quoteChar) inQuotes = false;
            else if (!inQuotes) { inQuotes = true; quoteChar = c; }
            else token += c;
        } else if (!inQuotes && delims.find(c) != string::npos) {
            if (!token.empty()) { result.push_back(token); token.clear(); }
        } else {
            token += c;
        }
    }
    if (!token.empty()) result.push_back(token);
    return result;
}





string replaceHomeDir(const string &path) {
    const char *home = getenv("HOME");
    string result = path;
    if (home && result.find(home) == 0) {
        result.replace(0, strlen(home), "~");
    }
    return result;
}
