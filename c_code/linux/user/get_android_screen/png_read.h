#ifndef __PNG_READ_H__
#define __PNG_READ_H__

struct png {
	unsigned char *image;
	unsigned int width;
	unsigned int height;
};

int png_read(char *file_stream, int length,
		void (*callback)(struct png *png, void *pdata), void *pdata);

#endif
