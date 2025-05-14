#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

void simple_pipe(const char* cmd1, char* const* argv1, const char* cmd2, char* const* argv2) {
	int pipefd[2], status;
	pid_t child1, child2;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	child1 = fork();
	if (child1 == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		execvp(cmd1, argv1);
		perror("execvp cmd1");
		exit(EXIT_FAILURE);
	}
	assert(child1 > 0);

	close(pipefd[1]);

	child2 = fork();
	if (child2 == 0) {
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		execvp(cmd2, argv2);
		perror("execvp cmd2");
		exit(EXIT_FAILURE);
	}
	assert(child2 > 0);

	close(pipefd[0]);
	waitpid(child1, &status, 0);
	waitpid(child2, &status, 0);
}

int main(int argc, char* argv[]) {
	if(argc==1 || argc>5){
		printf("Usage: ./<executable> <cmd1> <arg2> <cmd2> <arg2>\n");
		return -1;
	}
	char *list1[] = {" ", argv[2], NULL};
	char *list2[] = {" ", argv[4], NULL};
	simple_pipe(argv[1], list1, argv[3], list2);
	return 0;
}

