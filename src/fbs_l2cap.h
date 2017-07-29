#ifndef FBS_L2CAP_H__
#define FBS_L2CAP_H__

void FBS_l2cap_data_process(guchar *data, gint len);
void FBS_l2cap_comp_packets(guint16 handle, guint16 num);
void FBS_get_acl_mtu(guint16 *acl_mtu, guint16 *acl_max_pkt);
void FBS_get_sco_mtu(guint8 *sco_mtu, guint16 *sco_max_pkt);

#endif /* FBS_L2CAP_H__ */
