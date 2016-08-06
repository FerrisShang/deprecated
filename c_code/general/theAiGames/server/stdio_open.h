#ifndef __STDIO_OPEN__
#define __STDIO_OPEN__

struct stdio_pipe;

struct stdio_pipe* stdio_open(char *app, char *argv[]);
int stdio_read(struct stdio_pipe* pipe, char *buf, int len);
void stdio_write(struct stdio_pipe* pipe, char *data, int len);

#endif /* __STDIO_OPEN__ */
