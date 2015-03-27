#include "Constants.h"

#ifndef SHELL_H
#define SHELL_H

class Shell {
private:
    // Проверить, верна ли скобочная последовательность
    int isBalanced(char *str);

    // Выполнить complexCommand
    void executeComplexCommand(complexCommand *u);

    // Выполнить команду в фоновом процессе
    void executeInBackground(char *s);

    // Выбираем как выполнить команду - на шелле или на конвеере
    int executeSimpleCommand(char *s);

    // Выполняем команду на шелле
    int executeCommandOnShell(char *s);

    // Добавить simpleCommand
    simpleCommand *addSimpleCommand(simpleCommand *c, char *start, char *end, int usl);

    // Разворот для simpleCommand
    simpleCommand *rotateSimpleCommand(simpleCommand *c);

    // Вывести команду из simpleCommand
    void printSimpleCommand(simpleCommand *c);

    // Добавить complexCommand
    complexCommand *addComplexCommand(complexCommand *u, char *start, char *end, bool runInBackground);

    // Разворот для complexCommand
    complexCommand *rotateComplexCommand(complexCommand *u);

    // Вывести команду из complexCommand
    void printComplexCommand(complexCommand *u);

    // Добавить символ к строке
    void addSymbol(char **str, char ch, size_t *i, size_t *k);

    // Исполнение команды на конвеере
    int conveyor(char *s);

    // Разбиваем строку из аргументов на массив из аргументов
    char **splitArguments(char *s);

    // Прочитан символ "|"
    void pipeRead(size_t &i, size_t &k, bool &f1, bool &f2, char &ch, char *&str);

    // Прочитан символ " "
    void spaceRead(bool &f2, char &ch);

    // Прочитан символ "&"
    void andRead(size_t &i, size_t &k, bool &f1, bool &f2, char &ch, char *&str);

    // Прочитан символ ";"
    void semicolonRead(bool &f1, bool &f2, char &ch);

    // Прочитан любой несистемный символ
    void simpleSymbolRead(bool &f1, bool &f2, char &ch);

public:
    bool exitNow = false;
    bool setSignalToDefault = false;
    bool showInput = false;

    // Считать команду
    char *readCommand();

    // Запустить команду на нашем шелле
    void runShell(char *str);
};


#endif //SHELL_H
