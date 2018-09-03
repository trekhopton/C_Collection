#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "modifiedTime.h"
#include "hasChanged.h"
#include "doCommand.h" 

int main(int argc, char* argv[]){

	int c;
	bool verbose = false;
	while ((c = getopt(argc, argv, ":v")) != -1) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case '?':
			printf("unknown arg %c\n", optopt);
			exit(1);
			break;
		}
	}

	FILE* file = fopen("makefile", "r");
	char line[512];

	while (fgets(line, sizeof(line), file)) {

		if(hasChanged(line)){
			fgets(line, sizeof(line), file);
			while(strcmp(line, "\n") != 0){
				if(verbose){
					char* lineCopy = line;
					while(isspace((unsigned char)*lineCopy)) {
						lineCopy++;
					}
					printf("%s\n", lineCopy);
				}
				if(!doCommand(line)){
					exit(1);
				}
				if(fgets(line, sizeof(line), file) == 0){
					break;
				}
			}
		} else {
			while(strcmp(line, "\n") != 0){
				if(fgets(line, sizeof(line), file) == 0){
					break;
				}
			}
		}
	}

	fclose(file);

	return 0;
}
