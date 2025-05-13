#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
	int fd[2];
	char buf[30];
	if(pipe(fd) == -1) {
		perror("ERROR creating a pipe\n");
		exit(1);
	}
	if(!fork()) {
		printf("CHILD: writing to the pipe\n");
		write(fd[1], "Hello mother (or father?)", 26);
		printf("CHILD: exiting\n");
		exit(0);
	}
	else {
		printf("PARENT: reading from pipe\n");
		read(fd[0], buf, 26);
		printf("PARENT: read \"%s\"\n", buf);
		wait(NULL);
		printf("PARENT: exiting\n");
	}
	return 0;
}
