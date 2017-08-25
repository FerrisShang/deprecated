#ifndef __SCREEN_INFO_H__
#define __SCREEN_INFO_H__

#define SIZE_1920_1080

#define SC_PPATH " /sdcard/sc.tmp "
#define PT_PPATH " /sdcard/pt.tmp "
#define SH_PPATH " /sdcard/tmp.sh "
#define ADB_DEVICE " -s 04aafe300025fb47 "
#define ADB_SHELL  " adb "ADB_DEVICE" shell "
#define ADB_PUSH   " adb "ADB_DEVICE" push  "
#define ADB_GET_SC ADB_SHELL "\"screencap "SC_PPATH"; " \
					" sh "SH_PPATH" > /dev/null 2>&1; " \
					" cat "PT_PPATH"\""
#define ADB_PRESS(x,y) do{\
	char b[100]; \
	sprintf(b, ADB_SHELL "\"input tap %d %d \"", x, y); \
	system(b); \
}while(0)

#if defined SIZE_1920_1080

#define WIDTH  1080

#define SC_PICK_SH \
	 "dd if=/sdcard/sc.tmp of=/sdcard/pt.tmp " \
		  "bs=829440 count=1 seek=0 skip=9;\n" \
	 "dd if=/sdcard/sc.tmp of=/sdcard/pt.tmp " \
		  "bs=4320 count=1 seek=192 skip=1199;\n"

#define B_ENTRY_POSX   450
#define B_ENTRY_POSY   1200
#define B_LEVEL_POSX   540
#define B_LEVEL_POSY   1880
#define B_CONFIRM_POSX 540
#define B_CONFIRM_POSY 1200
#define B_EXIT_BT_POSX 540
#define B_EXIT_BT_POSY 1700
#define B_NONE_POSX    540
#define B_NONE_POSY    (WIDTH/2)
#define C_BASE_POSX    350
#define C_BASE_POSY    1700
#define C_WIDTH_POS    180

#elif defined SIZE_2560_1440
#else
#error "Screen size error"
#endif
#endif /* __SCREEN_INFO_H__ */
