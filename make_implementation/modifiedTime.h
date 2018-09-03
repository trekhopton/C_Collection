#ifndef MODIFIEDTIME_H
#define MODIFIEDTIME_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

time_t* modifiedTime(char const* path);

#endif