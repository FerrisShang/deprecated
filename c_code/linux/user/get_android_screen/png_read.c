#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "lodepng.h"
#include "png_read.h"

static struct png png;

int png_read(char *file_stream, int length,
		void (*callback)(struct png *png, void *pdata), void *pdata)
{
	unsigned int error;
	//error = lodepng_load_file(&p, &pngsize, filename);
	error = lodepng_decode32(&png.image, &png.width, &png.height, file_stream, length);
	if(error){
		printf("error %u: %s\n", error, lodepng_error_text(error));
		return -1;
	}else{
		/*use image here*/
		callback(&png, pdata);
		free(png.image);
		return 0;
	}
}
