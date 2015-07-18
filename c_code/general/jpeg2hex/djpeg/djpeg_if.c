#include <ctype.h>
#include "string.h"
#include "djpeg_if.h"
#include "cdjpeg.h"

int djpeg(struct djpeg_infile *infile, struct djpeg_outfile *outfile)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	djpeg_dest_ptr dest_mgr = NULL;
	JDIMENSION num_scanlines;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;
	jpeg_stdio_src(&cinfo, (FILE*)infile);
	if(jpeg_read_header(&cinfo, TRUE)<0)
		return -1;
	dest_mgr = jinit_write_ppm(&cinfo);
	dest_mgr->output_file = (FILE*)outfile;
	(void) jpeg_start_decompress(&cinfo);
	while (cinfo.output_scanline < cinfo.output_height) {
		num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
				dest_mgr->buffer_height);
		(*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
	}
	(void) jpeg_finish_decompress(&cinfo);
	outfile->width = cinfo.output_width;
	outfile->height = cinfo.output_height;
	jpeg_destroy_decompress(&cinfo);
	return 0;
}

size_t JFREAD(FILE *file,char *buf,ssize_t sizeofbuf)
{
	struct djpeg_infile *djpeg_infile = (struct djpeg_infile*)file;
	if(djpeg_infile->len >= sizeofbuf){
		memcpy(buf, djpeg_infile->buf, sizeofbuf);
		djpeg_infile->buf += sizeofbuf;
		djpeg_infile->len -= sizeofbuf;
		return sizeofbuf;
	}else{
		int tlen = djpeg_infile->len;
		memcpy(buf, djpeg_infile->buf, djpeg_infile->len);
		djpeg_infile->buf += sizeofbuf;
		djpeg_infile->len = 0;
		return tlen;
	}
}

size_t JFWRITE(FILE *file,char *buf,ssize_t sizeofbuf)
{
	struct djpeg_outfile *djpeg_outfile = (struct djpeg_outfile*)file;
	memcpy(djpeg_outfile->buf, buf, sizeofbuf);
	djpeg_outfile->buf += sizeofbuf;
	return sizeofbuf;
}
