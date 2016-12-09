#ifndef BLE_PROFILE_UUID_H__
#define BLE_PROFILE_UUID_H__

#define UINT8  unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned int

char* get_UUID_str(bt_uuid_t *uuid);
void get_props_str(char *props_str, int props);

void clear_chrc_handle(void);
int get_chrc_by_handle(bt_uuid_t *uuid, uint16_t handle);
int put_chrc_by_handle(bt_uuid_t *uuid, uint16_t handle);

char* conv_data_to_str(bt_uuid_t *uuid, char *buf, const uint8_t *value, uint16_t length);

#define STREAM_TO_UINT8(u8, p)   {\
	u8 = (UINT8)(*(p)); (p) += 1;\
}
#define STREAM_TO_UINT16(u16, p) {\
	u16 = ((UINT16)(UINT8)(*(p)) +\
			(((UINT16)(UINT8)(*((p) + 1))) << 8));\
	(p) += 2;\
}


#endif /* BLE_PROFILE_UUID_H__ */
