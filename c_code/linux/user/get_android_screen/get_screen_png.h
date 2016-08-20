#ifndef __GET_SCREEN_PNG_H__
#define __GET_SCREEN_PNG_H__

//#define STR_GET_SCREEN_PREFIX  "c93"
#define STR_GET_SCREEN_PREFIX
#define STR_GET_SCREEN_HEX_CMD "adb shell screencap -p"
//#define STR_GET_SCREEN_HEX_CMD "adb shell screencap | sed 's/\\\\r$//'"
#define SCREEN_HEX_FILE_NAME   "/tmp/fs.screencap.tmp.png"

struct screen_png {
	int size;
	char *data;
};

struct screen_png* get_screen_png(void);

#endif /* __GET_SCREEN_PNG_H__ */
