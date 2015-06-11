#include <string.h>
#include "enc_if.h"

s32_t encrypt_rsa(s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf)
{
	memcpy(ct_buf, pt_buf, pt_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%d called.\n", __func__);
	return pt_len;
}

s32_t decrypt_rsa(s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf)
{
	memcpy(pt_buf, ct_buf, ct_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%d called.\n", __func__);
	return ct_len;
}

s32_t encrypt_aes(s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf)
{
	memcpy(ct_buf, pt_buf, pt_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%d called.\n", __func__);
	return pt_len;
}

s32_t decrypt_aes(s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf)
{
	memcpy(pt_buf, ct_buf, ct_len);
	dbg_print(DBG_INFO, ENC_IF_DBG, "%d called.\n", __func__);
	return ct_len;
}

