#ifndef HASCHANGED_H
#define HASCHANGED_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include "modifiedTime.h"

bool hasChanged(const char* line);

#endif