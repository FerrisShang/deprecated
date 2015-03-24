#ifndef __LIST_FILE_H__
#define __LIST_FILE_H__

typedef int (*fileHandle)(const char *path, void*);
void List_Files(const char *path, fileHandle func, void *funcPara);

#endif
