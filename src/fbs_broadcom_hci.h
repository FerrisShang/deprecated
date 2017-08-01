#ifndef BROADCOM_HCI_H__
#define BROADCOM_HCI_H__

#define OCF_WRITE_BD_ADDR 0x0001
typedef struct {
	bdaddr_t	bdaddr;
} __attribute__ ((packed)) tFBS_write_bd_addr_cp;

#endif /* BROADCOM_HCI_H__ */
