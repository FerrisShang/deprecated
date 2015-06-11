#ifndef __ENC_IF_H__
#define __ENC_IF_H__

#include "base.h"


s32_t encrypt_rsa(s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf);
s32_t decrypt_rsa(s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf);
s32_t encrypt_aes(s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf);
s32_t decrypt_aes(s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf);

#endif /* __ENC_IF_H__ */
