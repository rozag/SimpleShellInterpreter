#ifndef CONSTANTS_H
#define CONSTANTS_H

struct complexCommand {
    char *s;
    bool runInBackground;
    complexCommand *next;
};

struct simpleCommand {
    char *s;
    int usl;
    simpleCommand *next;
};

#endif //CONSTANTS_H
