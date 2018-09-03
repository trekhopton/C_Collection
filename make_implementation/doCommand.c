#include "doCommand.h"

bool doCommand(const char* line){
		
	//Trim leading space
	while(isspace((unsigned char)*line)) {
		line++;
	}
	//Trim trailing space
	char* end;
	end = (char*)(line + strlen(line) - 1);
	while(end > line && isspace((unsigned char)*end)) end--;
	// Write new null terminator
	*(end+1) = 0;

	//make a copy of the line
	char lineCopy[512];
	strcpy(lineCopy, line);

	char* token;
	token = strtok(lineCopy, " ");

	// create arg array and store command as the first, get next token
	char* args[64];
	args[0] = token;
	token = strtok(NULL, " ");

	// everything is good
	bool allgood = true;

	// get the rest of the tokens
	int i = 1;
	while(token != NULL){

		args[i] = token;
		i++;
		token = strtok(NULL, " ");
	}
	args[i] = NULL;

	//fork and exec command
	pid_t pid;

	int status;

	if((pid = fork()) < 0){
		printf("Error: forking failed\n");
		allgood = false;
	} else if (pid == 0){
		if (execvp(args[0], args) < 0) {
			printf("Error: execvp failed\n");
			allgood = false;
		}
	} else {
		while (wait(&status) != pid);
	}
	
	return allgood;
}