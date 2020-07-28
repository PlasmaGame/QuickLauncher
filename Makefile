QL.exe: QL.c
	gcc -O2 -Wl,--subsystem,windows -o QL.exe QL.c