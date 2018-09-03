#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#define COPYMODE 0644
#define PATHNAMESIZE 1024
int bufferSize = 1024;

void copyFile(char*, char*);
int goCopy(char*);

int main(int argc, char* argv[]){

	int interactive = 0;
	int c;
	while ((c = getopt(argc, argv, ":i")) != -1) {
		switch(c) {
		case 'i':
			interactive = 1;
			break;
		case '?':
			printf("unknown arg %c\n", optopt);
			exit(2);
			break;
		}
	}

	struct stat pathStatus;
	stat(argv[argc-1], &pathStatus);

	if(S_ISDIR(pathStatus.st_mode)){ // if the last arg is a directory

		char *dir = argv[argc-1];

		int i;
		for(i = 1; i < argc-1; i++){

			if(interactive){
				if(goCopy(argv[i-1+optind]) == 1){
					char dirAndFile[PATHNAMESIZE];
					strcpy(dirAndFile, dir);
					strcat(dirAndFile, "/");
					strcat(dirAndFile, basename(argv[i-1+optind]));

					copyFile(argv[i-1+optind], dirAndFile);
				}
			} else {
				char dirAndFile[PATHNAMESIZE];
				strcpy(dirAndFile, dir);
				strcat(dirAndFile, "/");
				strcat(dirAndFile, basename(argv[i-1+optind]));

				copyFile(argv[i-1+optind], dirAndFile);
			}
		}
	} else if(argc == 3 + optind - 1){
		if(interactive){
			if(goCopy(argv[optind]) == 1){
				copyFile(argv[optind], argv[optind+1]);
			}
		} else {
			copyFile(argv[optind], argv[optind+1]);
		}
	} else {
		fprintf(stderr, "Error: wrong usage\n");
		exit(1);
	}
	return 0;
}

int goCopy(char* filename){

	printf("copy %s (y/n [n])\n", filename);

	char c[2];

	fgets(c, 2, stdin);

	if(strncmp(c, "y", 1) == 0 || strncmp(c, "Y", 1) == 0){
		return 1;
	} else {
		return 0;
	}
}

void copyFile(char* fromFile, char* toPath){

	int dest;
	int source;
	int charCount;
	char buffer[bufferSize];

	//open
	source = open(fromFile, O_RDONLY);
	if(source == -1){
		fprintf(stderr, "Error: cant open %s\n", fromFile);
		exit(1);
	}

	dest = creat(toPath, COPYMODE);
	if(dest == -1){
		fprintf( stderr, "Error: cant create %s\n", toPath);
		exit(1);
	}

	//copy
	while((charCount = read(source, buffer, bufferSize)) > 0){
		if(write(dest, buffer, charCount) != charCount){
			fprintf( stderr, "Error: cant write to %s\n", toPath);
			exit(1);
		}
	}

	if(charCount == -1){
		fprintf( stderr, "Error: cant read from %s\n", fromFile);
		exit(1);
	}

	//close
	if (close(source) == -1 || close(dest) == -1){
		fprintf( stderr, "Error: closing\n");
		exit(0);
	}
}
