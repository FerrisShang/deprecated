#include <fcntl.h>
#include <string.h>
#include "enc_if.h"

s32_t init_encrypt(sEncrypt_t *enc)
{
	enc->fd = open(ENCRYPT_DEV, O_RDWR);
	if(enc->fd > 0){
		enc->state = ENCRYPT_AES_UNINIT;
		return 0;
	}else{
		return 0;//just for test
		//return -1;
	}
}

s32_t update_aes(sEncrypt_t *enc, s8_t *ct_buf, s32_t ct_len)
{
	return 0;
}

s32_t encrypt_rsa(sEncrypt_t *enc, s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf)
{
	memcpy(ct_buf, pt_buf, pt_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%s called.\n", __func__);
	return pt_len;
}

s32_t decrypt_rsa(sEncrypt_t *enc, s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf)
{
	memcpy(pt_buf, ct_buf, ct_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%s called.\n", __func__);
	return ct_len;
}

s32_t encrypt_aes(sEncrypt_t *enc, s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf)
{
	memcpy(ct_buf, pt_buf, pt_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%s called.\n", __func__);
	return pt_len;
}

s32_t decrypt_aes(sEncrypt_t *enc, s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf)
{
	memcpy(pt_buf, ct_buf, ct_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%s called.\n", __func__);
	return ct_len;
}

