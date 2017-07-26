#ifndef FBS_PATCHRAM_H__
#define FBS_PATCHRAM_H__

#define FBS_PATCHRAM_SUCCESS         0
#define FBS_PATCHRAM_ERR_COMM       -1
#define FBS_PATCHRAM_ERR_OPEN_UART  -2
#define FBS_PATCHRAM_ERR_OPEN_FILE  -3
#define FBS_PATCHRAM_ERR_UNSUPPORT  -4

enum {
	FBS_PATCHRAM_TYPE_UART,
};

typedef struct {
	int type;
	union {
		struct {
			char dev_path[64];
			char fw_path[128];
		} uart;
	};
} tFBS_patchram;

int FBS_patchram(tFBS_patchram *info);

#endif /* FBS_PATCHRAM_H__ */
