#pragma once
#include <string>
#include <vector>

std::vector<std::string> splitWords(const std::string &input);
std::vector<std::string> getInternalCommands();
std::vector<std::string> getFiles(const std::string &prefix);
std::string commonPrefix(const std::vector<std::string> &words);
void setupAutocomplete();
