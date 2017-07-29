#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "fbs_l2cap.h"
#include "fbs_stack.h"

void FBS_l2cap_data_process(guchar *data, gint len)
{
}

void FBS_l2cap_comp_packets(guint16 handle, guint16 num)
{
}

void FBS_get_acl_mtu(guint16 *acl_mtu, guint16 *acl_max_pkt)
{
	*acl_mtu = 4096;
	*acl_max_pkt = 32;
}
void FBS_get_sco_mtu(guint8 *sco_mtu, guint16 *sco_max_pkt)
{
	*sco_mtu = 255;
	*sco_max_pkt = 32;
}
