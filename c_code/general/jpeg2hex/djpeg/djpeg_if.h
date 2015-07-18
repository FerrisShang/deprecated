#ifndef __DJPEG_IF_H__
#define __DJPEG_IF_H__
#include "stdio.h"

struct djpeg_infile{
	unsigned char *buf;
	int len;
};

struct djpeg_outfile{
	unsigned char *buf;
	int width;
	int height;
};

int djpeg(struct djpeg_infile *infile, struct djpeg_outfile *outfile);
size_t JFREAD(FILE *file,char *buf,ssize_t sizeofbuf);
size_t JFWRITE(FILE *file,char *buf,ssize_t sizeofbuf);

#endif /* __DJPEG_IF_H__ */
