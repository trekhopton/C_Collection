#include <stdlib.h>
#include <stdio.h>
#include "modifiedTime.h"
#include "hasChanged.h"
#include "doCommand.h" 

int main(int argc, char const *argv[]){

    // time_t t = *modifiedTime("tests/old.o");
    // static char timeStamp[32];
    // strftime(timeStamp, 32, "%x-%X", localtime(&t));
    // printf("%s\n", timeStamp);

    // printf("%s\n", modifiedTime("tests/old.o"));

    // char line[256] = "tests/old.o: tests/new.c tests/new.h";
    // char line[256] = "tests/new.o: tests/old.c tests/old.h";
    char line[256] = "ls -f";
    printf("%s\n", doCommand(line) ? "true" : "false");

    return 0;
}
