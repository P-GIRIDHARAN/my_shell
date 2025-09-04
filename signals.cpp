#include "signals.h"
#include <iostream>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

pid_t foregroundPid = -1;
std::vector<Job> jobs;

void sigintHandler(int sig) {
    (void)sig;
    if (foregroundPid > 0) {
        kill(-foregroundPid, SIGINT);
    } else {
        std::cout << "^C" << std::endl;
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void sigtstpHandler(int sig) {
    (void)sig;
    if (foregroundPid > 0) {
        kill(-foregroundPid, SIGTSTP);
        jobs.push_back({foregroundPid, "fg_process", "Stopped"});
        foregroundPid = -1;
    } else {
        std::cout << "^Z" << std::endl;
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void setupSignalHandlers() {
    struct sigaction sa_int, sa_tstp;

    sa_int.sa_handler = sigintHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, nullptr);

    sa_tstp.sa_handler = sigtstpHandler;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_tstp, nullptr);

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
}
