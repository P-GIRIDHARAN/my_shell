#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

extern vector<string> shellCommands;

bool isExecutable(const string &path);
vector<string> getExecutablesInPath(const string &prefix);
vector<string> getFilesInCWD(const string &prefix);
char *command_generator(const char *text, int state);
char **custom_completion(const char *text, int start, int end);
void display_matches_hook(char **matches, int count, int max_length);
void setupAutocomplete();

#endif
