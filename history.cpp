#include "history.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

static std::vector<std::string> historyBuffer;
static std::string historyFile = std::string(getenv("HOME")) + "/history.txt";


void loadHistory() {
    historyBuffer.clear();
    FILE* fin = fopen(historyFile.c_str(), "r");
    if (!fin) return;

    char line[1024];
    while (fgets(line, sizeof(line), fin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strlen(line) > 0)
            historyBuffer.push_back(std::string(line));
    }
    fclose(fin);

    if (historyBuffer.size() > MAX_HISTORY) {
        historyBuffer.erase(historyBuffer.begin(), historyBuffer.begin() + (historyBuffer.size() - MAX_HISTORY));
    }
}

void saveHistory() {
    FILE* fout = fopen(historyFile.c_str(), "w");
    if (!fout) return;

    size_t start = historyBuffer.size() > MAX_HISTORY ? historyBuffer.size() - MAX_HISTORY : 0;
    for (size_t i = start; i < historyBuffer.size(); ++i) {
        fprintf(fout, "%s\n", historyBuffer[i].c_str());
    }
    fclose(fout);
}

void addToHistory(const std::string& cmd) {
    if (cmd.empty()) return;

    if (!historyBuffer.empty() && historyBuffer.back() == cmd) return;

    historyBuffer.push_back(cmd);

    if (historyBuffer.size() > MAX_HISTORY)
        historyBuffer.erase(historyBuffer.begin());
}

void historyCommand(const std::vector<std::string>& args) {
    int num = 10;  
    if (args.size() == 2) {
        try { num = std::stoi(args[1]); }
        catch (...) { num = 10; }
    }

    int start = (historyBuffer.size() > static_cast<size_t>(num)) ? historyBuffer.size() - num : 0;
    for (size_t i = start; i < historyBuffer.size(); ++i) {
        std::cout << historyBuffer[i] << std::endl;
    }
}

std::vector<std::string> getHistory() {
    return historyBuffer;
}
