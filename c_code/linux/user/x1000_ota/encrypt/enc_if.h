#ifndef __ENC_IF_H__
#define __ENC_IF_H__

#include "base.h"


s32_t encrypt_rsa(u8_t *pt_buf, u32_t pt_len, u8_t *ct_buf);
s32_t decrypt_rsa(u8_t *ct_buf, u32_t ct_len, u8_t *pt_buf);
s32_t encrypt_aes(u8_t *pt_buf, u32_t pt_len, u8_t *ct_buf);
s32_t decrypt_aes(u8_t *ct_buf, u32_t ct_len, u8_t *pt_buf);

#endif /* __ENC_IF_H__ */
