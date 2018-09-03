modifiedTime.o: modifiedTime.c modifiedTime.h
	gcc -c -o modifiedTime.o modifiedTime.c

hasChanged.o: hasChanged.c hasChanged.h
	gcc -c -o hasChanged.o hasChanged.c

doCommand.o: doCommand.c doCommand.h
	gcc -c -o doCommand.o doCommand.c

spcMake.o: spcMake.c modifiedTime.h hasChanged.h doCommand.h
	gcc -c -o spcMake.o spcMake.c

spcMake: modifiedTime.o hasChanged.o spcMake.o doCommand.o
	gcc -o spcMake modifiedTime.o hasChanged.o spcMake.o doCommand.o