#include <iostream>
#include <cstring>
#include <signal.h>
#include "Shell.h"

int main(int argumentsCount, char *arguments[]) {
    signal(SIGINT, SIG_IGN);

    Shell shell = Shell();

    shell.setSignalToDefault = argumentsCount != 2;
    shell.showInput = argumentsCount < 2;

    char *str = NULL;
    while (true) {
        if (shell.showInput)
            std::cout << "> ";

        str = shell.readCommand();
//        cout << "DEBUG: " << str << endl;

        if (strcmp(str, "exit;") == 0) {
            return 0;
        }

        if ((!shell.exitNow) || ((strlen(str) != 1) || (str[0] != ';'))) {
            shell.runShell(str);
        }

        free(str);

        if (shell.exitNow) {
            return 0;
        }
    }
}
