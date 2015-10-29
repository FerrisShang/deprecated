#ifndef __FILE_UTIL_H__
#define __FILE_UTIL_H__

typedef int (*fileHandle)(const char *path, void*);
void List_Files(const char *path, fileHandle func, void *funcPara);
unsigned int get_file_size(const char *path);

#endif
