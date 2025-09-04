#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

// Built-in shell commands
extern vector<string> shellCommands;

// Function prototypes

// Check if a file is executable
bool isExecutable(const string &path);

// Get all executables in PATH that start with prefix
vector<string> getExecutablesInPath(const string &prefix);

// Get files in current directory matching prefix
vector<string> getFilesInCWD(const string &prefix);

// Readline generator function for autocompletion
char *command_generator(const char *text, int state);

// Custom completion function for readline
char **custom_completion(const char *text, int start, int end);

// Display matches immediately on one tab press
void display_matches_hook(char **matches, int count, int max_length);

// Setup readline autocomplete
void setupAutocomplete();

#endif // AUTOCOMPLETE_H
