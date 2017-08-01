#ifndef FBS_ACL_H__
#define FBS_ACL_H__

void FBS_acl_init(void);
void FBS_acl_data_process(guchar *data, gint len);
void FBS_acl_comp_packets(guint16 handle, guint16 num);
void FBS_get_acl_mtu(guint16 *acl_mtu, guint16 *acl_max_pkt);
void FBS_get_sco_mtu(guint8 *sco_mtu, guint16 *sco_max_pkt);
void FBS_record_chip_buffer_size(guint16 acl_mtu, guint16 acl_max_pkt,
		guint8 sco_mtu, guint16 sco_max_pkt);

#endif /* FBS_ACL_H__ */
