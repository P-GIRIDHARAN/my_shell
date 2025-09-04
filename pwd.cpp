#include "pwd.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void pwdCommand(const vector<string>& args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        perror("pwd");
        return;
    }

    string path(cwd);
    cout << replaceHomeDir(path) << endl;
}
