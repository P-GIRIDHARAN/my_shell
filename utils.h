#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::string trim(const std::string &str);
std::vector<std::string> tokenize(const std::string &str,
                                  const std::string &delim = " \t",
                                  bool preserveQuotes = false);
std::string replaceHomeDir(const std::string &path);
std::vector<std::string> tokenizeEcho(const std::string &str); 

#endif
