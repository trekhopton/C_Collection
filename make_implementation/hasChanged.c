#include "hasChanged.h"

bool hasChanged(const char* line){

	bool changed = false;

	//make a copy of the line
	char lineCopy[512];
	strcpy(lineCopy, line);

	char* token;
	token = strtok(lineCopy, ":");
	//store the dependant file
	char const* file = token;
	if(access(file, F_OK) == -1){
		changed = true;
	}
	token = strtok(NULL, " \t");

		
	while(token != NULL){

		double seconds = difftime(*modifiedTime(token), *modifiedTime(file));

		if(seconds > 0){
			changed = true;
		}

		token = strtok(NULL, " ");
	}
	return changed;
}