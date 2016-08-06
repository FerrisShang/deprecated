#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdio_open.h"


#define NUM_PIPES          2
#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1
/* always in a pipe[], pipe[0] is for read and pipe[1] is for write */
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD(pipes)  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD(pipes) ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define CHILD_READ_FD(pipes)   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD(pipes)  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

struct stdio_pipe {
	int pipes[NUM_PIPES][2];
	char *app;
	char *argv;
};

struct stdio_pipe* stdio_open(char *app, char *argv[])
{
	struct stdio_pipe *stdio_pipe;
	stdio_pipe = malloc(sizeof(struct stdio_pipe));
	if(!stdio_pipe){
		return NULL;
	}
	// pipes for parent to write and read
	pipe(stdio_pipe->pipes[PARENT_READ_PIPE]);
	pipe(stdio_pipe->pipes[PARENT_WRITE_PIPE]);

	if(!fork()) {
		dup2(CHILD_READ_FD(stdio_pipe->pipes), STDIN_FILENO);
		dup2(CHILD_WRITE_FD(stdio_pipe->pipes), STDOUT_FILENO);
		/* Close fds not required by child. Also, we don't
		   want the exec'ed program to know these existed */
		close(CHILD_READ_FD(stdio_pipe->pipes));
		close(CHILD_WRITE_FD(stdio_pipe->pipes));
		close(PARENT_READ_FD(stdio_pipe->pipes));
		close(PARENT_WRITE_FD(stdio_pipe->pipes));
		execv(app, argv);
	} else {
		/* close fds not required by parent */
		close(CHILD_READ_FD(stdio_pipe->pipes));
		close(CHILD_WRITE_FD(stdio_pipe->pipes));
		return stdio_pipe;
	}
}
int stdio_read(struct stdio_pipe* pipe, char *buf, int len)
{
	if(!pipe){
		return -1;
	}
	return read(PARENT_READ_FD(pipe->pipes), buf, len);
}
void stdio_write(struct stdio_pipe* pipe, char *data, int len)
{
	write(PARENT_WRITE_FD(pipe->pipes), data, len);
}
