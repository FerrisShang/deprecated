#ifndef __PARSER_UTILS_H__
#define __PARSER_UTILS_H__

#define UINT8  unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned int

#define UINT32_TO_STREAM(p, u32) {\
	*(p)++ = (UINT8)(u32);\
	*(p)++ = (UINT8)((u32) >> 8);\
	*(p)++ = (UINT8)((u32) >> 16);\
	*(p)++ = (UINT8)((u32) >> 24);\
}

#define UINT24_TO_STREAM(p, u24) {\
	*(p)++ = (UINT8)(u24);\
	*(p)++ = (UINT8)((u24) >> 8);\
	*(p)++ = (UINT8)((u24) >> 16);\
}

#define UINT16_TO_STREAM(p, u16) {\
	*(p)++ = (UINT8)(u16);\
	*(p)++ = (UINT8)((u16) >> 8);\
}

#define UINT8_TO_STREAM(p, u8) {\
	*(p)++ = (UINT8)(u8);\
}

#define ARRAY_TO_STREAM(p, a, len) {\
	register int ijk;\
	for (ijk = 0; ijk < len;ijk++)\
		*(p)++ = (UINT8) a[ijk];\
}

#define STREAM_TO_UINT8(u8, p)   {\
	u8 = (UINT8)(*(p)); (p) += 1;\
}

#define STREAM_TO_UINT16(u16, p) {\
	u16 = ((UINT16)(*(p)) +\
			(((UINT16)(*((p) + 1))) << 8));\
	(p) += 2;\
}

#define STREAM_TO_UINT24(u32, p) {\
	u32 = (((UINT32)(*(p))) +\
			((((UINT32)(*((p) + 1)))) << 8) +\
			((((UINT32)(*((p) + 2)))) << 16) );\
	(p) += 3;\
}

#define STREAM_TO_UINT32(u32, p) {\
	u32 = (((UINT32)(*(p))) +\
			((((UINT32)(*((p) + 1)))) << 8) +\
			((((UINT32)(*((p) + 2)))) << 16) +\
			((((UINT32)(*((p) + 3)))) << 24));\
	(p) += 4;\
}

#define STREAM_TO_ARRAY(a, p, len) {\
	register int ijk;\
	for (ijk = 0; ijk < len; ijk++)\
	((UINT8 *) a)[ijk] = *p++;\
}

#define REVERSE_STREAM_TO_ARRAY(a, p, len) {\
	register int ijk;\
	register UINT8 *_pa = (UINT8 *)a + len - 1;\
	for (ijk = 0; ijk < len; ijk++)\
	*_pa-- = *p++;\
}

#endif /* __PARSER_UTILS_H__ */
