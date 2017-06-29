#include <stdio.h>
#include <string.h>
#include <stdint.h>
void resample_mono(short* input_samples, int in_samples,
		short* output_samples, int out_samples)
{
	int osample;
	/* 16+16 fixed point math */
	uint32_t isample = 0;
	uint32_t istep = ((in_samples-2) << 16)/(out_samples-2);

#ifdef VERBOSE
	printf ("Audio : resample %d samples to %d\n",
			in_samples, out_samples);
#endif

	for (osample = 0; osample < out_samples - 1; osample++) {
		int s1;
		int s2;
		int16_t os;
		uint32_t t = isample&0xffff;

		/* don't "optimize" the (isample >> 16)*2 to (isample >> 15) */
		s1 = input_samples[(isample >> 16)];
		s2 = input_samples[(isample >> 16)+1];

		os = (s1 * (0x10000-t)+ s2 * t) >> 16;
		output_samples[osample] = os;

		isample += istep;
	}
	output_samples[out_samples-1] = input_samples[in_samples-1];
}
void dump(short *buf, int len)
{
	int i;
	for(i=0;i<len;i++){
		if((i%16)==0){printf("\n");}
		printf("%3d ", (unsigned short)buf[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	short buf1[17] = {
		10,20,30,40,50,
		60,70,80,90,100,
		90,95,120,180,20,
		90,15,
	};
    short buf2[27] = {
		10,20,30,40,50,
		60,70,80,90,100,
		90,95,120,180,20,
		90,15,120,180,20,
		60,70,80,90,100,
		190,100,
	};
	short out[25];

	dump(buf1,sizeof(buf1)/sizeof(short));
	resample_mono(buf1, sizeof(buf1)/sizeof(short), out, sizeof(out)/sizeof(short));
	dump(out,sizeof(out)/sizeof(short));
	dump(buf2,sizeof(buf2)/sizeof(short));
	resample_mono(buf2, sizeof(buf2)/sizeof(short), out, sizeof(out)/sizeof(short));
	dump(out,sizeof(out)/sizeof(short));
}
