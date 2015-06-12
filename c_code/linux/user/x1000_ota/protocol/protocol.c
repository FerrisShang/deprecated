#include <string.h>
#include <time.h>
#include <unistd.h>
#include "base.h"
#include "protocol.h"
#include "para_if.h"
#include "enc_if.h"

static s32_t pack_valid_data(sEncrypt_t *enc, s8_t *buf, u32_t time_offset,
							eOta_cmd_t cmd, s8_t *data, s32_t dat_len);
static s32_t pack_comm_data(sEncrypt_t *enc, s8_t *buf, eEncrypt_t mode, s8_t *data, s32_t dat_len);
static s32_t check_value_data(s8_t *buf, s32_t len);

static s32_t pack_valid_data(sEncrypt_t *enc, s8_t *buf, u32_t time_offset,
							  eOta_cmd_t cmd, s8_t *data, s32_t dat_len)
{
	s32_t total_len, i;
	s32_t time_stamp;
	//header
	buf[VALID_DATA_HEADER_POS] = VALID_DATA_HEADER[0];
	buf[VALID_DATA_HEADER_POS+1] = VALID_DATA_HEADER[1];
	//len
	total_len = 4 + 2 + dat_len + 1;//time_stamp + cmd + dat_len + check_sum
	buf[VALID_DATA_LEN_POS] = (total_len >> 8) & 0xFF;
	buf[VALID_DATA_LEN_POS+1] = total_len & 0xFF;
	//time_stamp
	time_stamp = time_offset + time(NULL);
	buf[VALID_DATA_TIME_STAMP_POS] = (time_stamp >> 24) & 0xFF;
	buf[VALID_DATA_TIME_STAMP_POS+1] = (time_stamp >> 16) & 0xFF;
	buf[VALID_DATA_TIME_STAMP_POS+2] = (time_stamp >> 8) & 0xFF;
	buf[VALID_DATA_TIME_STAMP_POS+3] = time_stamp & 0xFF;
	//cmd
	buf[VALID_DATA_CMD_POS] = (((u16_t)cmd) >> 8) & 0xFF;
	buf[VALID_DATA_CMD_POS+1] = ((u16_t)cmd) & 0xFF;
	//data
	memcpy(&buf[VALID_DATA_DATA_POS], data, dat_len);
	//check_sum
	buf[VALID_DATA_DATA_POS+dat_len] = 0;
	for(i=0;i<VALID_DATA_DATA_POS+dat_len;i++){
		buf[VALID_DATA_DATA_POS+dat_len] += buf[i];
	}
	return VALID_DATA_DATA_POS+dat_len+1; //1 means check_sum
}

static s32_t pack_comm_data(sEncrypt_t *enc, s8_t *buf, eEncrypt_t mode, s8_t *data, s32_t dat_len)
{
	s32_t comm_dat_len;
	//header
	buf[COMM_DATA_HEADER_POS] = COMM_DATA_HEADER[0];
	//encrypt mode
	buf[COMM_DATA_TYPE_POS] = (s8_t)mode;
	//valid data
	if(mode == TYPE_ENCRYPT_NULL){
		memcpy(&buf[COMM_DATA_VALID_DATA_POS], data, dat_len);
		comm_dat_len = dat_len;
	}else if(mode == TYPE_ENCRYPT_RSA){
		comm_dat_len = encrypt_rsa(enc, data, dat_len, &buf[COMM_DATA_VALID_DATA_POS]);
	}else if(mode == TYPE_ENCRYPT_AES){
		comm_dat_len = encrypt_aes(enc, data, dat_len, &buf[COMM_DATA_VALID_DATA_POS]);
	}else{
		dbg_print(DBG_ERROR, PRO_DBG, "unsupport encrypt mode.\n");
		return 0;
	}
	if(comm_dat_len <= 0){
		dbg_print(DBG_ERROR, PRO_DBG, "package communication data failed.\n");
		return 0;
	}
	//data len
	buf[COMM_DATA_LEN_POS] = (comm_dat_len >> 8) & 0xFF;
	buf[COMM_DATA_LEN_POS+1] = comm_dat_len & 0xFF;
	return COMM_DATA_VALID_DATA_POS + comm_dat_len;
}

static s32_t check_value_data(s8_t *buf, s32_t len)
{
	u32_t i;
	s8_t check_sum = 0;
	for(i=0;i<len-1;i++){
		check_sum += buf[i];
	}
	if(check_sum == buf[len-1])
		return 0;
	else
		return -1;
}

sProtocol_t* protocol_create(eEncrypt_t mode)
{
	sProtocol_t* pro;
	pro = (sProtocol_t*)mem_malloc(sizeof(sProtocol_t));
	if(pro == NULL){
		dbg_print(DBG_ERROR, PRO_DBG, "not enough memory.\n");
		goto alloc_pro_error;
	}
	memset(pro, 0,sizeof(sProtocol_t));
	pro->valid_dat_buf = mem_malloc(VALID_DATA_MAX_LEN);
	if(pro->valid_dat_buf == NULL){
		dbg_print(DBG_ERROR, PRO_DBG, "allocate buffer failed. not enough memory.\n");
		goto alloc_data_buf_error;
	}
	memset(pro->valid_dat_buf, 0, VALID_DATA_MAX_LEN);
	pro->mode = mode;
	if(mode != TYPE_ENCRYPT_NULL){
		if(init_encrypt(&pro->enc)<0){
			dbg_print(DBG_ERROR, PRO_DBG, "open encrypt device error.\n");
			goto open_dev_error;
		}
	}
	return pro;

open_dev_error:
	mem_free(pro->valid_dat_buf);
alloc_data_buf_error:
	mem_free(pro);
alloc_pro_error:
	return NULL;
}

void protocol_destory(sProtocol_t *pro)
{
	if(pro == NULL){
		dbg_print(DBG_WARNING, PRO_DBG, "attempt to free zero pointer.\n");
	}else{
		if(pro->valid_dat_buf){
			mem_free(pro->valid_dat_buf);
		}
		if(pro->mode != TYPE_ENCRYPT_NULL){
			if(pro->enc.fd){
				close(pro->enc.fd);
			}
		}
		mem_free(pro);
	}
}

s32_t pack_req_encrypt_comm(sProtocol_t *pro, s8_t *buf, s8_t *device_id)
{
	s32_t valid_dat_len;
	s32_t comm_dat_len;

	valid_dat_len = pack_valid_data(&pro->enc, pro->valid_dat_buf, pro->time_stamp_offset, CMD_REQ_ENCRYPT_COMM,
			device_id, PARA_DEVICE_ID_LEN);
	comm_dat_len = pack_comm_data(&pro->enc, buf, TYPE_ENCRYPT_RSA, pro->valid_dat_buf, valid_dat_len);
	return comm_dat_len;
}

s32_t pack_req_new_key(sProtocol_t *pro, s8_t *buf, s8_t *device_id)
{
	s32_t valid_dat_len;
	s32_t comm_dat_len;

	valid_dat_len = pack_valid_data(&pro->enc, pro->valid_dat_buf, pro->time_stamp_offset, CMD_REQ_NEW_KEY,
			device_id, PARA_DEVICE_ID_LEN);
	comm_dat_len = pack_comm_data(&pro->enc, buf, TYPE_ENCRYPT_RSA, pro->valid_dat_buf, valid_dat_len);
	return comm_dat_len;
}

s32_t pack_req_activition(sProtocol_t *pro, s8_t *buf, s8_t *device_id, s8_t *wifi_addr, s8_t *bt_addr)
{
	s32_t valid_dat_len;
	s32_t comm_dat_len;
	s8_t *data_buf, *p;
	data_buf = mem_malloc(VALID_DATA_MAX_LEN);
	if(data_buf == NULL){
		dbg_print(DBG_ERROR, PRO_DBG, "not enough memory.\n");
		return 0;
	}
	p = data_buf;
	memcpy(p, device_id, PARA_DEVICE_ID_LEN);
	p += PARA_DEVICE_ID_LEN;
	memcpy(p, wifi_addr, PARA_WIFI_ADDR_LEN);
	p += PARA_WIFI_ADDR_LEN;
	memcpy(p, bt_addr, PARA_BT_ADDR_LEN);
	p += PARA_BT_ADDR_LEN;

	valid_dat_len = pack_valid_data(&pro->enc, pro->valid_dat_buf, pro->time_stamp_offset, CMD_REQ_ACTIVITION,
			data_buf, p-data_buf);
	if(pro->mode == TYPE_ENCRYPT_NULL){
		comm_dat_len = pack_comm_data(&pro->enc, buf, TYPE_ENCRYPT_NULL, pro->valid_dat_buf, valid_dat_len);
	}else{
		comm_dat_len = pack_comm_data(&pro->enc, buf, TYPE_ENCRYPT_AES, pro->valid_dat_buf, valid_dat_len);
	}
	mem_free(data_buf);
	return comm_dat_len;
}

s32_t pack_req_new_version(sProtocol_t *pro, s8_t *buf, s8_t *device_id, s8_t *version)
{
	s32_t valid_dat_len;
	s32_t comm_dat_len;
	s8_t *data_buf, *p;
	data_buf = mem_malloc(VALID_DATA_MAX_LEN);
	if(data_buf == NULL){
		dbg_print(DBG_ERROR, PRO_DBG, "not enough memory.\n");
		return 0;
	}
	p = data_buf;
	memcpy(p, device_id, PARA_DEVICE_ID_LEN);
	p += PARA_DEVICE_ID_LEN;
	memcpy(p, version, PARA_WIFI_ADDR_LEN);
	p += PARA_VERSION_LEN;

	valid_dat_len = pack_valid_data(&pro->enc, pro->valid_dat_buf, pro->time_stamp_offset, CMD_REQ_NEW_VERSION,
			data_buf, p-data_buf);
	if(pro->mode == TYPE_ENCRYPT_NULL){
		comm_dat_len = pack_comm_data(&pro->enc, buf, TYPE_ENCRYPT_NULL, pro->valid_dat_buf, valid_dat_len);
	}else{
		comm_dat_len = pack_comm_data(&pro->enc, buf, TYPE_ENCRYPT_AES, pro->valid_dat_buf, valid_dat_len);
	}
	mem_free(data_buf);
	return comm_dat_len;
}

s32_t parse_package(sProtocol_t *pro, s8_t *buf, s32_t buf_len,
						 eOta_cmd_t *cmd, s8_t *data, s32_t *data_len)
{
	s8_t *data_buf, *p = 0;
	s32_t valid_data_len;
	s32_t time_stamp;
	data_buf = mem_malloc(VALID_DATA_MAX_LEN);
	if(data_buf == NULL){
		dbg_print(DBG_ERROR, PRO_DBG, "not enough memory.\n");
		return PARSE_OTHER_ERROR;
	}
	if(buf[COMM_DATA_HEADER_POS] != COMM_DATA_HEADER[0]){
		dbg_print(DBG_WARNING, PRO_DBG, "parse package error(header:\"0x%02X\" error,not \"0x%02X\").\n",
				(u8_t)buf[COMM_DATA_HEADER_POS], COMM_DATA_HEADER[0]);
		mem_free(data_buf);
		return PARSE_STYLE_ERROR;
	}
	valid_data_len = (buf[COMM_DATA_LEN_POS]<<8)+buf[COMM_DATA_LEN_POS+1];
	if(valid_data_len+COMM_DATA_VALID_DATA_POS != buf_len){
		dbg_print(DBG_WARNING, PRO_DBG, "parse package error(comm data length is wrong).\n");
		mem_free(data_buf);
		return PARSE_STYLE_ERROR;
	}
	//get value data
	if(buf[COMM_DATA_TYPE_POS] == TYPE_ENCRYPT_NULL){
		p = &buf[COMM_DATA_VALID_DATA_POS];
	}else if(buf[COMM_DATA_TYPE_POS] == TYPE_ENCRYPT_RSA){
		valid_data_len = decrypt_rsa(&pro->enc, &buf[COMM_DATA_VALID_DATA_POS], valid_data_len, data_buf);
		if(valid_data_len <= 0){
			mem_free(data_buf);
			return PARSE_DECRYPT_ERROR;
		}
		p = data_buf;
	}else if(buf[COMM_DATA_TYPE_POS] == TYPE_ENCRYPT_AES){
		valid_data_len = decrypt_aes(&pro->enc, &buf[COMM_DATA_VALID_DATA_POS], valid_data_len, data_buf);
		if(valid_data_len <= 0){
			mem_free(data_buf);
			return PARSE_DECRYPT_ERROR;
		}
		p = data_buf;
	}else{
		dbg_print(DBG_WARNING, PRO_DBG, "unsupport encrypt type\n");
		mem_free(data_buf);
		return PARSE_DECRYPT_ERROR;
	}
	//check valid data
	if(valid_data_len < VALID_DATA_TIME_STAMP_POS || //length check
			p[VALID_DATA_HEADER_POS] != VALID_DATA_HEADER[0] || //header check
			p[VALID_DATA_HEADER_POS+1] != VALID_DATA_HEADER[1] ||
			//data length check
			(p[VALID_DATA_LEN_POS]<<8)+(p[VALID_DATA_LEN_POS+1])+VALID_DATA_TIME_STAMP_POS != valid_data_len){
		mem_free(data_buf);
		dbg_print(DBG_INFO, PRO_DBG, "valid data format error.header=\"%c%c\"\n",
					p[VALID_DATA_HEADER_POS], p[VALID_DATA_HEADER_POS+1]);
		return PARSE_STYLE_ERROR;
	}
	if(check_value_data(p, valid_data_len) != 0){
		mem_free(data_buf);
		dbg_print(DBG_INFO, PRO_DBG, "valid data checksum error.\n");
		return PARSE_CHECKSUM_ERROR;
	}
	//parse valid data
	//time_stamp_offset
	time_stamp = (p[VALID_DATA_TIME_STAMP_POS]<<24)+(p[VALID_DATA_TIME_STAMP_POS]<<16)+
					(p[VALID_DATA_TIME_STAMP_POS]<<8)+(p[VALID_DATA_TIME_STAMP_POS]);
	pro->time_stamp_offset = time_stamp - time(0);
	//command
	*cmd = (eOta_cmd_t)((p[VALID_DATA_CMD_POS]<<8) + p[VALID_DATA_CMD_POS+1]);
	//data
	*data_len = valid_data_len - VALID_DATA_DATA_POS - 1;
	memcpy(data, &p[VALID_DATA_DATA_POS], *data_len);
	mem_free(data_buf);
	return PARSE_SUCCESS;
}
