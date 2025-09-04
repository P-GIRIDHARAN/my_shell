#ifndef SIGNALS_H
#define SIGNALS_H

#include <csignal>
#include <sys/types.h>

extern pid_t foregroundPid;

void setupSignalHandlers();

#endif
