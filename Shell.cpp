#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "Shell.h"

using namespace std;

int Shell::isBalanced(char *str) {
    int counter = 0;
    size_t l = strlen(str);
    for (int i = 0; i < l; i++)
        if (str[i] == '(') counter++;
        else if (str[i] == ')') {
            counter--;
            if (counter < 0) return -1;
        }

    return counter;
}

void Shell::runShell(char *str) {
    if (isBalanced(str)) {
        cout << "Не сбалансировано" << endl;
        return;
    }

    int n = 0, start = 0, counter = 0;
    size_t l = strlen(str);
    complexCommand *u = NULL;

    for (int i = 0; i < l; i++)
        if (counter) {
            if (str[i] == ')') counter--;
            if (str[i] == '(') counter++;
        } else {
            if (str[i] == '(') counter++;
            if (str[i] == ';') {
                n++;
                u = addComplexCommand(u, str + start, str + i - 1, false);
                start = i + 1;
            }

            if (str[i] == '&') {
                if (str[i + 1] != '&') {
                    n++;
                    u = addComplexCommand(u, str + start, str + i - 1, true);
                    start = i + 1;
                } else {
                    i++;
                }
            }
        }

    u = rotateComplexCommand(u);

    while (u != NULL) {
        executeComplexCommand(u);
        u = (*u).next;
    }
}

void Shell::executeComplexCommand(complexCommand *u) {
    char *str = (*u).s;
    if ((*u).runInBackground) {
        executeInBackground(str);
        return;
    }
    int i, n = 0, start = 0, counter = 0;
    size_t l = strlen(str);
    simpleCommand *c = NULL;

    for (i = 0; i < l; i++)
        if (counter) {
            if (str[i] == ')') counter--;
            if (str[i] == '(') counter++;
        } else {
            if (str[i] == '(') counter++;
            if (str[i] == '&') {
                n++;
                c = addSimpleCommand(c, str + start, str + i - 1, 0);
                i++;
                start = i + 1;
            }
            if ((str[i] == '|') && (str[i + 1] == '|')) {
                n++;
                c = addSimpleCommand(c, str + start, str + i - 1, -1);
                i++;
                start = i + 1;
            }
        }

    c = addSimpleCommand(c, str + start, str + l - 1, 1);
    c = rotateSimpleCommand(c);
    int ex;
    while (c != NULL) {
        ex = executeSimpleCommand((*c).s);
        while ((c != NULL) && ((*c).usl != ex) && ((*c).usl != 1)) {
            c = (*c).next;
        }
        c = (*c).next;
    }
}

void Shell::executeInBackground(char *s) {
    int k1;
    if ((k1 = fork()) == 0) {
        if (fork() == 0) {
            int fd[2];
            pipe(fd);
            write(fd[1], s, strlen(s));
            close(fd[1]);
            dup2(fd[0], 0);
            close(fd[0]);
            int fdnull = open("/dev/null", O_WRONLY);
            dup2(fdnull, 1);
            close(fdnull);
            execlp("./mz6", "./mz6", "fon", (char *) 0);
            cout << "Ошибка" << endl;
            return;
        } else {
            exit(0);
        }
    } else {
        waitpid(k1, NULL, 0);
    }
}

int Shell::executeSimpleCommand(char *s) {
    if (s[0] == '(') {
        return executeCommandOnShell(s);
    } else {
        return conveyor(s);
    }
}

int Shell::executeCommandOnShell(char *s) {
    int k2;
    if ((k2 = fork()) == 0) {
        int fd[2];
        pipe(fd);
        while (s[strlen(s) - 1] == ' ')s[strlen(s) - 1] = 0;
        write(fd[1], s + 1, strlen(s) - 2);
        close(fd[1]);
        dup2(fd[0], 0);
        close(fd[0]);
        execlp("./mz6", "./mz6", "1", "2", (char *) 0);
        cout << "Ошибка" << endl;
        exit(127);
    } else {
        int status;
        waitpid(k2, &status, 0);
        if (WIFSIGNALED (status) || (WIFEXITED (status) && WEXITSTATUS (status)))
            return -1;
    }

    return 0;
}

simpleCommand *Shell::addSimpleCommand(simpleCommand *c, char *start, char *end, int usl) {
    simpleCommand *t = (simpleCommand *) malloc(sizeof(simpleCommand));
    (*t).usl = usl;
    char *s = NULL;
    if (end - start + 1) {
        s = (char *) malloc(sizeof(char) * (end - start + 1));
        int i = 0;
        while (start != end) {
            s[i] = *start;
            start++;
            i++;
        }
        s[i] = *start;
        s[i + 1] = 0;
    } else {
        s = (char *) malloc(sizeof(char));
        *s = 0;
    }
    (*t).s = s;
    (*t).next = c;

    return t;
};

simpleCommand *Shell::rotateSimpleCommand(simpleCommand *c) {
    simpleCommand *t = NULL, *p = NULL;
    while (c != NULL) {
        p = c;
        c = (*c).next;
        (*p).next = t;
        t = p;
    }

    return t;
}

void Shell::printSimpleCommand(simpleCommand *c) {
    if (c != NULL) {
        cout << (*c).s << endl;
        printSimpleCommand((*c).next);
    }
}

complexCommand *Shell::addComplexCommand(complexCommand *u, char *start, char *end, bool runInBackground) {
    complexCommand *t = (complexCommand *) malloc(sizeof(complexCommand));
    (*t).runInBackground = runInBackground;
    char *s = NULL;
    if (end - start + 1) {
        s = (char *) malloc(sizeof(char) * (end - start + 1));
        int i = 0;
        while (start != end) {
            s[i] = *start;
            start++;
            i++;
        }
        s[i] = *start;
        s[i + 1] = 0;
    } else {
        s = (char *) malloc(sizeof(char));
        *s = 0;
    }
    (*t).s = s;
    (*t).next = u;

    return t;
};

complexCommand *Shell::rotateComplexCommand(complexCommand *u) {
    complexCommand *t = NULL, *p = NULL;
    while (u != NULL) {
        p = u;
        u = (*u).next;
        (*p).next = t;
        t = p;
    }

    return t;
}

void Shell::printComplexCommand(complexCommand *u) {
    if (u != NULL) {
        cout << (*u).s << endl;
        printComplexCommand((*u).next);
    }
}

void Shell::addSymbol(char **str, char ch, size_t *i, size_t *k) {
    *(*str + *i) = ch;
    (*i)++;
    if ((*i) == (*k)) {
        (*k) <<= 1;
        (*str) = (char *) realloc(*str, (*k));
    }
}

char *Shell::readCommand() {
    size_t i = 0;
    size_t stringSize = 16;
    bool flagOne = true;
    bool flagTwo = true;

    char symbol = (char) getchar();
    char *str = (char *) malloc(sizeof(char) * stringSize);

    while (symbol != EOF) {
        if ((flagOne && (symbol == '\n')) || (flagTwo && (symbol == ' '))) {
            symbol = (char) getchar();
        } else {
            if (symbol == '\n')break;
            addSymbol(&str, symbol, &i, &stringSize);
            if (symbol == '|') pipeRead(i, stringSize, flagOne, flagTwo, symbol, str);
            else if (symbol == ' ') spaceRead(flagTwo, symbol);
            else if (symbol == '&') andRead(i, stringSize, flagOne, flagTwo, symbol, str);
            else if (symbol == ';') semicolonRead(flagOne, flagTwo, symbol);
            else simpleSymbolRead(flagOne, flagTwo, symbol);
        }
    }

    if (!((str[i - 1] == ';') || (str[i - 1] == '&')))addSymbol(&str, ';', &i, &stringSize);
    addSymbol(&str, 0, &i, &stringSize);
    str = (char *) realloc(str, i);
    if (symbol == EOF) exitNow = true;

    return str;
}

void Shell::simpleSymbolRead(bool &f1, bool &f2, char &ch) {
    f1 = false;
    f2 = false;
    ch = (char) getchar();
}

void Shell::semicolonRead(bool &f1, bool &f2, char &ch) {
    f2 = true;
    f1 = false;
    ch = (char) getchar();
}

void Shell::andRead(size_t &i, size_t &k, bool &f1, bool &f2, char &ch, char *&str) {
    if ((ch = (char) getchar()) == '&') {
        addSymbol(&str, ch, &i, &k);
        f1 = true;
        f2 = true;
        ch = (char) getchar();
    } else {
        f2 = true;
        f1 = false;
    }
}

void Shell::spaceRead(bool &f2, char &ch) {
    f2 = true;
    ch = (char) getchar();
}

void Shell::pipeRead(size_t &i, size_t &k, bool &f1, bool &f2, char &ch, char *&str) {
    if ((ch = (char) getchar()) == '|') {
        addSymbol(&str, ch, &i, &k);
        f1 = true;
        f2 = true;
        ch = (char) getchar();
    } else if ((ch == ' ') || (ch == '\n')) {
        addSymbol(&str, ' ', &i, &k);
        f1 = true;
        f2 = true;
        ch = (char) getchar();
    }
}

int Shell::conveyor(char *s) {
    int pipesCount = 1, i;
    size_t l = strlen(s);

    for (i = 0; i < l; i++)
        if (s[i] == '|') pipesCount++;

    char *arr[pipesCount], *start = s;
    s[l] = '|';
    pipesCount = 0;

    for (i = 0; i <= l; i++)
        if (s[i] == '|') {
            arr[pipesCount] = start;
            s[i] = 0;
            start = s + i + 1;
            pipesCount++;
        }

    start = arr[pipesCount - 1];
    l = strlen(start);

    for (i = 0; i < l; i++)
        if ((start[i] == '>') || (start[i] == '<')) {
            start += i;
            break;
        }

    int k, j;
    char *in = NULL;
    int input = 0, output = 0;

    while ((start[0] == '>') || (start[0] == '<')) {
        if ((start[0] == '<'))k = 0; else if ((start[0] == '>') && (start[1] == '>')) {
            k = 2;
            start[0] = 0;
            start++;
        } else k = 1;

        start[0] = 0;
        start++;
        i = 0;

        while (start[0] == ' ')start++;

        while ((start[i] != ' ') && (start[i] != '>') && (start[i] != '<') && (start[i] != 0))i++;

        in = (char *) malloc(sizeof(char) * (i + 1));

        for (j = 0; j < i; j++) in[j] = start[j];

        in[i] = 0;
        start += i;

        while (start[0] == ' ') start++;

        if (k == 0) {
            input = open(in, O_RDONLY);
            if (input < 0) {
                cout << "Файл не найден" << endl;
                return -1;
            }
        }

        if (k == 1) {
            output = open(in, O_CREAT | O_WRONLY | O_APPEND, 0666);
            if (output < 0) {
                cout << "Ошибка файла" << endl;
                return -1;
            }
        }

        if (k == 2) {
            output = open(in, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (output < 0) {
                cout << "Ошибка файла" << endl;
                return -1;
            }
        }

        free(in);
    }

    char **p = NULL;
    int status;
    if (pipesCount == 1) {
        // Только один пайп
        if ((k = fork()) == 0) {
            if (setSignalToDefault) signal(SIGINT, SIG_DFL);
            if (input) {
                dup2(input, 0);
                close(input);
            }
            if (output) {
                dup2(output, 1);
                close(output);
            }
            p = splitArguments(arr[0]);
            execvp(p[0], p);
            cout << "Неверная команда" << endl;
            exit(127);
        }

        waitpid(k, &status, 0);

        if (WIFSIGNALED (status) || (WIFEXITED (status) && WEXITSTATUS (status)))
            return -1;
        if (input)
            close(input);
        if (output)
            close(output);
    } else {
        int fd1[2], fd2[2];
        if (pipe(fd1)) {
            cout << "Ошибка канала" << endl;
            return -1;
        }
        if (!fork()) {
            if (setSignalToDefault)signal(SIGINT, SIG_DFL);
            dup2(fd1[1], 1);
            close(fd1[0]);
            close(fd1[1]);
            if (input) {
                dup2(input, 0);
                close(input);
            }
            p = splitArguments(arr[0]);
            execvp(p[0], p);
            cout << "Неверная команда" << endl;
            exit(127);
        }
        close(fd1[1]);
        if (input)close(input);
        for (i = 1; i < pipesCount - 1; i++) {
            if (pipe(fd1)) {
                cout << "Ошибка канала" << endl;
                return -1;
            }
            pid_t pid = fork();
            if (pid < 0) {
                cout << "Ошибка процесса" << endl;
                return -1;
            }
            if (pid == 0) {
                if (setSignalToDefault)signal(SIGINT, SIG_DFL);
                dup2(fd1[0], 0);
                close(fd1[0]);
                dup2(fd2[1], 1);
                close(fd2[1]);
                close(fd2[0]);
                p = splitArguments(arr[i]);
                execvp(p[0], p);
                cout << "Неверная команда" << endl;
                exit(127);
            }
            close(fd1[0]);
            close(fd2[1]);
            dup2(fd2[0], fd1[0]);
            close(fd2[0]);
        }
        int lastpid = fork();
        if (lastpid < 0) {
            cout << "Ошибка процесса" << endl;
            return -1;
        }
        if (lastpid == 0) {
            if (setSignalToDefault)signal(SIGINT, SIG_DFL);
            dup2(fd1[0], 0);
            close(fd1[0]);
            if (output) {
                dup2(output, 1);
                close(output);
            }
            p = splitArguments(arr[pipesCount - 1]);
            execvp(p[0], p);
            cout << "Неверная команда" << endl;
            exit(127);
        }
        close(fd1[0]);
        if (output)close(output);
        waitpid(lastpid, &status, 0);
        if (WIFSIGNALED (status) || (WIFEXITED (status) && WEXITSTATUS (status)))
            return -1;
        for (i = 0; i < (pipesCount - 1); i++) {
            wait(NULL);
        }
    }
    return 0;
}

char **Shell::splitArguments(char *s) {
    int i, n = 0;
    while (s[0] == ' ')s++;
    while (s[strlen(s) - 1] == ' ')s[strlen(s) - 1] = 0;
    size_t l = strlen(s);
    s[l] = ' ';
    for (i = 0; i <= l; i++)
        if (s[i] == ' ')n++;
    char **p = (char **) malloc(sizeof(char *) * (n + 1)), *start = s;
    n = 0;
    for (i = 0; i <= l; i++)
        if (s[i] == ' ') {
            p[n] = start;
            start = s + i + 1;
            s[i] = 0;
            n++;
        }
    p[n] = NULL;

    return p;
}

