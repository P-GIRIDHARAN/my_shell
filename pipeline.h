#pragma once
#include <vector>
#include <string>

void handlePipeline(const std::vector<std::string>& commands, bool background);
void executeCommandWithRedirection(const std::vector<std::string>& args,
                                   const std::string& inputFile,
                                   const std::string& outputFile,
                                   bool append);