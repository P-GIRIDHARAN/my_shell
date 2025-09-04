#include "echo.h"
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

void echoCommand(const vector<string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        cout << args[i];
        if (i != args.size() - 1) cout << " ";
    }
    cout << endl;
}
