#ifndef SIGNALS_H
#define SIGNALS_H

#include <csignal>
#include <sys/types.h>
#include <vector>
#include <string>

struct Job {
    pid_t pid;
    std::string name;
    std::string state;
};

extern pid_t foregroundPid;
extern std::vector<Job> jobs;

void sigintHandler(int sig);
void sigtstpHandler(int sig);
void setupSignalHandlers();

#endif
