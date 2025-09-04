#pragma once
#include <vector>
#include <string>
#include <unistd.h>

struct Job {
    pid_t pid;
    std::string name;
    std::string status;
};

extern pid_t foregroundPid;
extern std::vector<Job> jobs;

void sigintHandler(int sig);
void sigtstpHandler(int sig);
void setupSignalHandlers();
