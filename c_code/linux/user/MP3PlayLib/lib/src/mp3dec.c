#include "mp3dec.h"
#include <string.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;

static const int br_tbl[3][3][16] = {
	{// MPEG-1
		// Layer1
		{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 },
		// Layer2
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0 },
		// Layer3
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 },
	},
	{// MPEG-2
		// Layer1
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0 },
		// Layer2
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 },
		// Layer3
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 },
	},
	{// MPEG-2.5
		// Layer1 (not available)
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0 },
		// Layer2 (not available)
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 },
		// Layer3
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 },
	},
};

static const int fr_tbl[3][4] = {
	// MPEG-1
	{ 44100, 48000, 32000, 0/*reserved*/ },
	// MPEG-2
	{ 22050, 24000, 16000, 0/*reserved*/ },
	// MPEG-2.5
	{ 11025, 12000, 8000, 0/*reserved*/ },
};
typedef void (*FRAME_PROC) (MPEG_HEADER* h, char* mpeg, char* pcm);

MPEG_DECODE_OPTION m_option;
FRAME_PROC m_frame_proc;
int m_last_error;
int m_frequency;
int m_frame_size, m_pcm_size;
int m_enableEQ;
float m_equalizer[32];
//float m_band_tbl[2][32];

void L3table_init();
void L3decode_start(MPEG_HEADER* h);
void L3decode_frame(MPEG_HEADER* h, char* mpeg, char* pcm);

void mp3DecodeInit()
{
	m_option.reduction = 0;
	m_option.convert = 0;
	m_option.freqLimit = 24000;

	L3table_init();
}

int mp3GetHeader(char* buf, MPEG_HEADER* h)
{
	h->version		= (buf[1] & 0x08) >> 3;
	h->layer		= (buf[1] & 0x06) >> 1;
	h->error_prot	= (buf[1] & 0x01);
	h->br_index		= (buf[2] & 0xf0) >> 4;
	h->fr_index		= (buf[2] & 0x0c) >> 2;
	h->padding		= (buf[2] & 0x02) >> 1;
	h->extension	= (buf[2] & 0x01);
	h->mode			= (buf[3] & 0xc0) >> 6;
	h->mode_ext		= (buf[3] & 0x30) >> 4;
	h->copyright	= (buf[3] & 0x08) >> 3;
	h->original		= (buf[3] & 0x04) >> 2;
	h->emphasis		= (buf[3] & 0x03);

	if (buf[0] != (char)0xFF)	{//sync error
		m_last_error = MP3_ERROR_INVALID_SYNC;
		return 0;
	}
	if ((buf[1] & (char)0xF0) == (char)0xF0)		//MPEG-1, MPEG-2
		h->version = (h->version) ? 1 : 2;
	else if ((buf[1] & (char)0xF0) == (char)0xE0)	//MPEG-2.5
		h->version = 3;
	else {
		m_last_error = MP3_ERROR_INVALID_SYNC;
		return 0;
	}
	if (h->fr_index >= 3 ||
			h->br_index == 0 || h->br_index >= 15 ||
			h->layer == 0 || h->layer >= 4) {
		m_last_error = MP3_ERROR_INVALID_HEADER;
		return 0;
	}
	h->layer = 4 - h->layer;
	h->error_prot = (h->error_prot) ? 0 : 1;

	return 1;
}

int mp3GetLastError()
{
	return m_last_error;
}

int mp3FindSync(char* buf, int size, int* sync)
{
	int i;

	*sync = 0;
	size -= 3;
	if (size <= 0) {
		m_last_error = MP3_ERROR_OUT_OF_BUFFER;
		return 0;
	}
	for (i = 0; i < size; i ++) {
		if (buf[i] == (char)0xFF) {
			if ((buf[i + 1] & (char)0xF0) == (char)0xF0) {
				break;
			}
			else if ((buf[i + 1] & (char)0xF0) == (char)0xE0) {
				break;
			}
		}
	}
	if (i == size) {
		m_last_error = MP3_ERROR_OUT_OF_BUFFER;
		return 0;
	}
	*sync = i;
	return 1;
}

void mp3GetDecodeOption(MPEG_DECODE_OPTION* option)
{
	*option = m_option;
}

int mp3SetDecodeOption(MPEG_DECODE_OPTION* option)
{
	m_option = *option;
	return 1;
}

int mp3SetEqualizer(int* value)
{
	int i;
	if (value == (void*)0) {
		m_enableEQ = 0;
		return 1;
	}
	m_enableEQ = 1;
	//60, 170, 310, 600, 1K, 3K
	for (i = 0; i < 6; i ++) {
		m_equalizer[i] = (float)pow(10,(double)value[i]/200);
	}
	//6K
	m_equalizer[6] = (float)pow(10,(double)value[6]/200);
	m_equalizer[7] = m_equalizer[6];
	//12K
	m_equalizer[8] = (float)pow(10,(double)value[7]/200);
	m_equalizer[9] = m_equalizer[8];
	m_equalizer[10] = m_equalizer[8];
	m_equalizer[11] = m_equalizer[8];
	//14K
	m_equalizer[12] = (float)pow(10,(double)value[8]/200);
	m_equalizer[13] = m_equalizer[12];
	m_equalizer[14] = m_equalizer[12];
	m_equalizer[15] = m_equalizer[12];
	m_equalizer[16] = m_equalizer[12];
	m_equalizer[17] = m_equalizer[12];
	m_equalizer[18] = m_equalizer[12];
	m_equalizer[19] = m_equalizer[12];
	//16K
	m_equalizer[20] = (float)pow(10,(double)value[9]/200);
	m_equalizer[21] = m_equalizer[20];
	m_equalizer[22] = m_equalizer[20];
	m_equalizer[23] = m_equalizer[20];
	m_equalizer[24] = m_equalizer[20];
	m_equalizer[25] = m_equalizer[20];
	m_equalizer[26] = m_equalizer[20];
	m_equalizer[27] = m_equalizer[20];
	m_equalizer[28] = m_equalizer[20];
	m_equalizer[29] = m_equalizer[20];
	m_equalizer[30] = m_equalizer[20];
	m_equalizer[31] = m_equalizer[20];
	return 1;
}

#define VBR_FRAMES_FLAG		0x0001
#define VBR_BYTES_FLAG		0x0002
#define VBR_TOC_FLAG		0x0004
#define VBR_SCALE_FLAG		0x0008

static int extractInt4(char* buf)
{// big endian extract
	return buf[3] | (buf[2] << 8) |
		(buf[1] << 16) | (buf[0] << 24);
}

int mp3GetDecodeInfo(char* mpeg, int size, MPEG_DECODE_INFO* info, int decFlag)
{
	MPEG_HEADER* h = &info->header;
	char* p = mpeg;
	int vbr;
	uint minBitRate, maxBitRate;
	uint i, j, flags;


	//int bitRate;
	//int frame_size;

	if (size < 156) {//max vbr header size
		m_last_error = MP3_ERROR_OUT_OF_BUFFER;
		return 0;
	}
	if (!mp3GetHeader(p, h)) {
		return 0;
	}
	//check VBR Header
	p += 4;//skip mpeg header
	if (h->error_prot) p += 2;//skip crc
	if (h->layer == 3) {//skip side info
		if (h->version == 1) {//MPEG-1
			if (h->mode != 3) p += 32;
			else p += 17;
		}
		else {//MPEG-2, MPEG-2.5
			if (h->mode != 3) p += 17;
			else p += 9;
		}
	}

	info->bitRate = br_tbl[h->version-1][h->layer-1][h->br_index] * 1000;
	info->frequency = fr_tbl[h->version-1][h->fr_index];
	if (memcmp(p, "Xing", 4) == 0) {//VBR
		p += 4;
		flags = extractInt4(p);
		p += 4;
		if (!(flags & (VBR_FRAMES_FLAG | VBR_BYTES_FLAG))) {
			m_last_error = MP3_ERROR_INVALID_HEADER;
			return 0;
		}
		info->frames = extractInt4(p);
		p += 4;
		info->dataSize = extractInt4(p);
		p += 4;
		if (flags & VBR_TOC_FLAG) p += 100;
		if (flags & VBR_SCALE_FLAG) p += 4;
		vbr = 1;
		minBitRate = 0xffffffff;
		maxBitRate = 0;
		for (i = 1; i < 15; i ++) {
			j = br_tbl[h->version-1][h->layer-1][i] * 1000;
			if (j < minBitRate) minBitRate = j;
			if (j > maxBitRate) maxBitRate = j;
		}
	}
	else if (memcmp(p, "VBRI", 4) == 0) {//VBRI
		p += 10;
		info->dataSize = extractInt4(p);
		p += 4;
		info->frames = extractInt4(p);
		p += 4;
		vbr = 1;
		minBitRate = 0xffffffff;
		maxBitRate = 0;
		for (i = 1; i < 15; i ++) {
			j = br_tbl[h->version-1][h->layer-1][i] * 1000;
			if (j < minBitRate) minBitRate = j;
			if (j > maxBitRate) maxBitRate = j;
		}
	}
	else {//not VBR
		minBitRate = 0xffffffff;
		maxBitRate = 0;
		vbr = 0;
		info->frames = 0;
		//info->skipSize = 0;
		info->dataSize = 0;
		//info->bitRate = br_tbl[h->version-1][h->layer-1][h->br_index] * 1000;
	}

	//	info->frequency = fr_tbl[h->version-1][h->fr_index];
	//	info->msPerFrame = ms_p_f_table[h->layer-1][h->fr_index];
	//	if (h->version == 3) info->msPerFrame *= 2;
	switch (h->layer) {
		case 1://layer1
			info->outputSize = 384 >> m_option.reduction;
			if (!vbr) {
				info->skipSize = 0;
				info->minInputSize = (12 * info->bitRate / info->frequency) * 4;//one slot is 4 bytes long
				info->maxInputSize = info->minInputSize + 4;
			}
			else {
				info->skipSize = (12 * info->bitRate / info->frequency + h->padding) * 4;
				info->minInputSize = (12 * minBitRate / info->frequency) * 4;
				info->maxInputSize = (12 * maxBitRate / info->frequency) * 4 + 4;
			}
			break;
		case 2://layer2
			info->outputSize = 1152 >> m_option.reduction;
			if (!vbr) {
				info->skipSize = 0;
				info->minInputSize = 144 * info->bitRate / info->frequency;
				info->maxInputSize = info->minInputSize + 1;
			}
			else {
				info->skipSize = 144 * info->bitRate / info->frequency + h->padding;
				info->minInputSize = 144 * minBitRate / info->frequency;
				info->maxInputSize = 144 * maxBitRate / info->frequency + 1;
			}
			break;
		case 3://layer3
			i = (h->version == 1) ? 1 : 2;
			//info->outputSize = 1152 >> m_option.reduction;
			info->outputSize = (1152 >> m_option.reduction) / i;
			if (!vbr) {
				info->skipSize = 0;
				info->minInputSize = 144 * info->bitRate / info->frequency / i;
				info->maxInputSize = info->minInputSize + 1;
			}
			else {
				info->skipSize = 144 * info->bitRate / info->frequency / i + h->padding;
				info->minInputSize = 144 * minBitRate / info->frequency / i;
				info->maxInputSize = 144 * maxBitRate / info->frequency / i + 1;
			}
			break;
	}

	if ((h->mode == 3) || (m_option.convert & 3))
		info->channels = 1;
	else
		info->channels = 2;
	if (m_option.convert & 8) {
		//not available
		info->bitsPerSample = 8;
		info->outputSize *= info->channels;
	}
	else {
		info->bitsPerSample = 16;
		info->outputSize *= info->channels * 2;
	}
	if ( decFlag == 1 ) {
		m_frequency = info->frequency;
		m_pcm_size = info->outputSize;
	}
	info->frequency >>= m_option.reduction;
	if (vbr) info->bitRate = 0;

	return 1;
}

void sbt_init();

int mp3DecodeStart(char* mpeg, int size)
{
	MPEG_DECODE_INFO info;
	MPEG_HEADER* h = &info.header;

	if (!mp3GetDecodeInfo(mpeg, size, &info, 1)) {
		return 0;
	}

	sbt_init();
	switch (h->layer) {
		case 1:
		case 2:
			break;
		case 3:
			L3decode_start(h);
			m_frame_proc = L3decode_frame;
			break;
	}
	return 1;
}

int mp3DecodeFrame(MPEG_DECODE_PARAM* param)
{
	MPEG_HEADER* h = &param->header;

	if (param->inputSize <= 4) {
		m_last_error = MP3_ERROR_OUT_OF_BUFFER;
		return 0;
	}
	if (!mp3GetHeader((char*)param->inputBuf, h)) {
		return 0;
	}

	param->bitRate = br_tbl[h->version-1][h->layer-1][h->br_index] * 1000;
	switch (h->layer) {
		case 1://layer1
			m_frame_size = (12 * param->bitRate / m_frequency + h->padding) * 4;
			break;
		case 2://layer2
			m_frame_size = 144 * param->bitRate / m_frequency + h->padding;
			break;
		case 3://layer3
			if (h->version == 1) m_frame_size = 144 * param->bitRate / m_frequency + h->padding;
			else m_frame_size = (144 * param->bitRate / m_frequency) / 2 + h->padding;
			break;
	}
	if (param->inputSize < m_frame_size) {
		m_last_error = MP3_ERROR_OUT_OF_BUFFER;
		return 0;
	}
	m_frame_proc(h, (char*)param->inputBuf, (char*)param->outputBuf);
	param->inputSize = m_frame_size;
	param->outputSize = m_pcm_size;
	return 1;
}

void null_frame_proc(MPEG_HEADER* h, char* mpeg, char* pcm) {}
void L3decode_reset();

void mp3MuteStart(MPEG_DECODE_PARAM* param)
{
	m_frame_proc = null_frame_proc;
}

void mp3MuteEnd(MPEG_DECODE_PARAM* param)
{
	MPEG_HEADER* h = &param->header;

	switch (h->layer) {
		case 1:
		case 2:
		case 3:
			L3decode_reset();
			m_frame_proc = L3decode_frame;
			break;
	}
}
