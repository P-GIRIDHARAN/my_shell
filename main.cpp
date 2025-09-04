#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "ls.h"
#include "pinfo.h"
#include "search.h"
#include "io_redirection.h"
#include "pipeline.h"
#include "background.h"
#include "signals.h"
#include "autocomplete.h"
#include "history.h"
#include "utils.h"
#include <termios.h>
#include <unistd.h>

using namespace std;
extern std::vector<std::string> historyBuffer; 
struct termios orig_termios;
int historyPos = -1;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void printPromptAndInput(const string& prompt, const string& input) {
    cout << "\r\033[K";       
    cout << prompt << input;   
    cout.flush();
}
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    raw.c_lflag |= ISIG;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


string getUsername() {
    struct passwd* pw = getpwuid(getuid());
    return string(pw->pw_name);
}

string getHostName() {
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    return string(hostname);
}

string getCurrentDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    string dir(cwd);
    const char* home = getenv("HOME");
    if (home && dir.find(home) == 0) {
        dir.replace(0, strlen(home), "~");
    }
    return dir;
}

string readInputWithHistory(vector<string>& history, int& historyPos) {
    string input;
    int cursorPos = 0;
    historyPos = history.size();
    enableRawMode();

    auto redrawLine = [&]() {
        string prompt = getUsername() + "@" + getHostName() + ":" + getCurrentDir() + "> ";
        cout << "\r\033[K" << prompt << input;
        int moveLeft = input.size() - cursorPos;
        if (moveLeft > 0) cout << "\033[" << moveLeft << "D";
        cout.flush();
    };
    redrawLine();
    char c;
    while (true) {
        if (read(STDIN_FILENO, &c, 1) != 1) break;

        if (c == '\n') { 
            cout << endl;
            break;
        } 
        else if (c == 127) { 
            if (cursorPos > 0) {
                input.erase(input.begin() + cursorPos - 1);
                cursorPos--;
                redrawLine();
            }
        } 
        else if (c == 27) {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': 
                        if (historyPos > 0) {
                            historyPos--;
                            input = history[historyPos];
                            cursorPos = input.size();
                            redrawLine();
                        }
                        break;
                    case 'B': 
                        if (historyPos < (int)history.size() - 1) {
                            historyPos++;
                            input = history[historyPos];
                        } else {
                            historyPos = history.size();
                            input.clear();
                        }
                        cursorPos = input.size();
                        redrawLine();
                        break;
                    case 'C':
                        if (cursorPos < (int)input.size()) {
                            cursorPos++;
                            cout << "\033[C";
                            cout.flush();
                        }
                        break;
                    case 'D': 
                        if (cursorPos > 0) {
                            cursorPos--;
                            cout << "\033[D";
                            cout.flush();
                        }
                        break;
                }
            }
        } 
        else if (c == 3) { 
            input.clear();
            cursorPos = 0;
            cout << endl << "^C" << endl;  
            cout.flush();
            redrawLine();                   
        } 
        else if (c == 26) { 
            input.clear();
            cursorPos = 0;
            cout << endl << "^Z" << endl;
            cout.flush();
            raise(SIGTSTP);                
        } 
        else if (c == 4) { 
            if (input.empty()) {
                cout << endl;
                disableRawMode();
                exit(0);
            }
        }
        else if (c == '\t') { 
            vector<string> words = splitWords(input.substr(0, cursorPos));
            string prefix = words.empty() ? "" : words.back();
            vector<string> matches;

            if (words.size() == 1) {  
                matches = getInternalCommands();
            } else {                  
                matches = getFiles(prefix);
            }

            vector<string> filtered;
            for (auto &m : matches)
                if (m.find(prefix) == 0)
                    filtered.push_back(m);

            if (filtered.empty()) continue;

            string completion = commonPrefix(filtered);

            if (!completion.empty() && completion.size() > prefix.size()) {
                input.insert(cursorPos, completion.substr(prefix.size()));
                cursorPos += completion.size() - prefix.size();
                redrawLine();
            } else if (filtered.size() == 1) {  
                input.insert(cursorPos, filtered[0].substr(prefix.size()));
                cursorPos += filtered[0].size() - prefix.size();
                redrawLine();
            } else { 
                cout << endl;
                for (auto &f : filtered) cout << f << "  ";
                cout << endl;
                redrawLine();
            }
        }

        else { 
            input.insert(input.begin() + cursorPos, c);
            cursorPos++;
            redrawLine();
        }
    }


    disableRawMode();
    return input;
}

void executeCommand(string cmd) {
    cmd = trim(cmd);
    if (cmd.empty()) return;

    bool background = false;
    if (cmd.back() == '&') {
        background = true;
        cmd.pop_back();
        cmd = trim(cmd);
    }
    if (cmd.find('|') != string::npos) {
        vector<string> pipeCmds = tokenize(cmd, "|");
        handlePipeline(pipeCmds);
        return;
    }

    vector<string> args = tokenize(cmd, " \t");
    if (args.empty()) return;

    bool hasRedirection = false;
    for (const auto& s : args) {
        if (s == "<" || s == ">" || s == ">>") {
            hasRedirection = true;
            break;
        }
    }
    if (hasRedirection) {
        handleIORedirection(args);
        return;
    }
    string command = args[0];
    if (command == "exit") {
        exit(0);  
    }
    if (command == "cd") {
        cdCommand(args);
    }
     else if (command == "pwd") {
        pwdCommand(args);
    } else if (command == "echo") {
        echoCommand(args);
    } 
    else if (command == "ls") {
        lsCommand(args);
    } 
    else if (command == "pinfo") {
        pinfoCommand(args);
    } 
    else if (command == "search") {
        searchCommand(args);
    }
     else if (command == "history") {
        historyCommand(args);
    } 
    else {
        runSystemCommand(args, background);
    }
    addToHistory(cmd);

}

int main() {
    setupSignalHandlers();
    atexit(saveHistory);

    loadHistory();  

    string username = getUsername();
    string hostname = getHostName();

    while (true) {
        cout << username << "@" << hostname << ":" << getCurrentDir() << "> ";
        string input = readInputWithHistory(historyBuffer, historyPos);
        vector<string> commands = tokenize(input, ";");

        for (auto &cmd : commands) {
            executeCommand(cmd);
        }
    }
    return 0;
}
