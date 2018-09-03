#include "modifiedTime.h"

time_t* modifiedTime(char const* path){

	static struct stat info;
	stat(path, &info);

	return &(info.st_mtime);
}