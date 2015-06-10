#include <string.h>
#include "enc_if.h"

s32_t encrypt_rsa(u8_t *pt_buf, u32_t pt_len, u8_t *ct_buf)
{
	memcpy(ct_buf, pt_buf, pt_len);
	return pt_len;
}

s32_t decrypt_rsa(u8_t *ct_buf, u32_t ct_len, u8_t *pt_buf)
{
	memcpy(pt_buf, ct_buf, ct_len);
	return ct_len;
}

s32_t encrypt_aes(u8_t *pt_buf, u32_t pt_len, u8_t *ct_buf)
{
	memcpy(ct_buf, pt_buf, pt_len);
	return pt_len;
}

s32_t decrypt_aes(u8_t *ct_buf, u32_t ct_len, u8_t *pt_buf)
{
	memcpy(pt_buf, ct_buf, ct_len);
	return ct_len;
}

