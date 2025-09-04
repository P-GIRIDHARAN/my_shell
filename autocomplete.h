#pragma once
#include <string>
#include <vector>
using namespace std;

vector<string> splitWords(const string &input);
vector<string> getInternalCommands();
vector<string> getFiles(const string &prefix);
string commonPrefix(const vector<string> &words);
