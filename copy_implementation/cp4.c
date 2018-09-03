#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#define COPYMODE 0777
#define PATHNAMESIZE 1024
int bufferSize = 1024;

void copyFile(char*, char*);
void copyDir(char*, char*);
void copyItem(char*, char*);
int goCopy(char*);

int main(int argc, char* argv[]){

	int interactive = 0;
	int recursive = 0;
	int c;
	while ((c = getopt(argc, argv, ":ir")) != -1) {
		switch(c) {
		case 'i':
			interactive = 1;
			break;
		case 'r':
			recursive = 1;
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

		char *destDir = argv[argc-1];

		int i;
		for(i = 1; i < argc-optind; i++){
			//unless it's interactive and the user says no, proceed
			if(!(interactive && goCopy(argv[i-1+optind]) == 0)){

				struct stat iStatus;
				stat(argv[i-1+optind], &iStatus);

				if(!recursive && S_ISDIR(iStatus.st_mode)){
					printf("Skipping Directory %s\n", argv[i-1+optind]);
				} else {
					copyItem(argv[i-1+optind], destDir);
				}
			}
		}
	} else if(argc == 2 + optind){
		//unless it's interactive and the user says no, proceed
		if(!(interactive && goCopy(argv[optind]) == 0)){
			copyFile(argv[optind], argv[optind+1]);
		}
	} else {
		fprintf(stderr, "Error: wrong usage\n");
		exit(1);
	}
	return 0;
}

void copyItem(char* fromSource, char* toDir){

	struct stat sourceStatus;
	stat(fromSource, &sourceStatus);

	if(S_ISDIR(sourceStatus.st_mode)){

		copyDir(fromSource, toDir);

	} else if(S_ISREG(sourceStatus.st_mode)){

		char toDirAndFile[PATHNAMESIZE];
		strcpy(toDirAndFile, toDir);
		strcat(toDirAndFile, "/");
		strcat(toDirAndFile, basename(fromSource));

		copyFile(fromSource, toDirAndFile);

	}
}

// from actual dir to copy - to dir to contain copied dir
void copyDir(char* fromDir, char* toDir){

	char newDir[PATHNAMESIZE];
	strcpy(newDir, toDir);
	strcat(newDir, "/");
	strcat(newDir, basename(fromDir));
	mkdir(newDir, COPYMODE);

	//open dir to copy
	DIR *fdir;
	struct dirent *sd;
	fdir = opendir(fromDir);
	if(fdir == NULL){
		fprintf(stderr, "Error: unable to open fdir: %s\n", fromDir);
		exit(1);
	}
	// read what's in dir to copy
	while((sd = readdir(fdir)) != NULL){

		if(!(strcmp(sd->d_name, ".") == 0 || strcmp(sd->d_name, "..") == 0)){

			char toCopy[PATHNAMESIZE];
			strcpy(toCopy, fromDir);
			strcat(toCopy, "/");
			strcat(toCopy, sd->d_name);
			copyItem(toCopy, newDir);
		}
	}
	closedir(fdir);
}

int goCopy(char* filename){

	printf("copy %s (y/n [n])\n", filename);

	char c[1];

	fgets(c, 3, stdin);

	if(strncmp(c, "y", 1) == 0 || strncmp(c, "Y", 1) == 0){
		return 1;
	} else {
		return 0;
	}
}

void copyFile(char* fromFile, char* toFile){

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

	dest = creat(toFile, COPYMODE);
	if(dest == -1){
		fprintf( stderr, "Error: cant create %s\n", toFile);
		exit(1);
	}

	//copy
	while((charCount = read(source, buffer, bufferSize)) > 0){
		if(write(dest, buffer, charCount) != charCount){
			fprintf( stderr, "Error: cant write to %s\n", toFile);
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
		exit(1);
	}
}
