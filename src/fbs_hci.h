#ifndef FBS_HCI_H__
#define FBS_HCI_H__

#include "fbs_stack.h"
#include "fbs_hci_type.h"

/* HCI Packet types */
#define FBS_UART_COMMAND_PKT     0x01
#define FBS_UART_ACLDATA_PKT     0x02
#define FBS_UART_SCODATA_PKT     0x03
#define FBS_UART_EVENT_PKT       0x04
#define FBS_UART_RESERVE         0xFF

#define FBS_UART_MSG_TYPE(p)     (p[0])

typedef struct {
	;
} tFBS_hci_evt;

typedef gboolean (*tFBS_hci_cb)(guint8 evt, guint8 *data, guint16 len);

void FBS_hci_reg_callback(tFBS_hci_cb callback);
void FBS_hci_send(guint16 opcode, gpointer *data, gint len);
void FBS_hci_evt_process(guchar *data, gint len);

#endif /* FBS_HCI_H__ */
