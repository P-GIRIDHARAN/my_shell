#include "signals.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t foregroundPid = -1;

void sigintHandler(int sig) {
    if (foregroundPid > 0) {
        kill(foregroundPid, SIGINT);   
    }
}

void sigtstpHandler(int sig) {
    if (foregroundPid > 0) {
        kill(foregroundPid, SIGTSTP);  
    }
}

void setupSignalHandlers() {
    signal(SIGINT, sigintHandler);   
    signal(SIGTSTP, sigtstpHandler); 
}
