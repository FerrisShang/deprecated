#ifndef __SHOOTIMAE_H__
#define __SHOOTIMAE_H__

/* shootimage for PAWSCAM board */
int shootimage(unsigned char *path, int count, int interval);

/* shootimage_icam for iCam board */
int shootimage_icam(unsigned char *path, int w, int h);

/* shootimage_m is main function */
int shootimage_m(unsigned char *path, int count, int interval, int i_fmt, int w, int h);

#endif
