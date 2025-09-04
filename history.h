#pragma once
#include <vector>
#include <string>

inline constexpr int MAX_HISTORY = 20;

void addToHistory(const std::string& cmd);
void historyCommand(const std::vector<std::string>& args);
std::vector<std::string> getHistory();
void loadHistory();
void saveHistory();
