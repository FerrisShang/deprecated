/*****************************************************************************
**
**  Name:           app_ble_client.h
**
**  Description:    Bluetooth BLE include file
**
**  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef APP_BLE_CLIENT_H
#define APP_BLE_CLIENT_H

#include "bsa_api.h"
#include "app_ble.h"
#include "app_ble_client_db.h"

typedef struct {
	UINT16 service_uuid;	/* Enter Service UUID to read(eg. x1800) */
	UINT16 char_uuid;	/* Enter Char UUID to read(eg. x2a00) */
	UINT16 descr_id;	/* Enter Descriptor type UUID to read(eg. x2902) */
	int client_num;
	int is_descript;	/* Select descriptor? (yes=1 or no=0),default 0 */
	int ser_inst_id;	/* Enter Instance ID for Service UUID(eg. 0,1,2..), default 0 */
	int char_inst_id;	/* Enter Instance ID for Char UUID(eg. 0,1,2..) */
	int is_primary;		/* Enter Is_primary value(eg:0,1) */
} ble_client_read_data;

typedef struct {
	UINT16 service_uuid;	/* Service UUID to write */
	UINT16 char_uuid;	/* Char UUID to write */
	UINT16 descr_id;	/* Descriptor type UUID to write(eg. x2902) */
	UINT8 desc_value;	/* Descriptor value to write(eg. x01) */
	UINT8 write_data[BSA_BLE_CL_WRITE_MAX];		/* write data */
	UINT16 write_len;	/* write length: bytes */
	UINT8 write_type;	/* 1-GATT_WRITE_NO_RSP 2-GATT_WRITE */
	int client_num;
	int is_descript;	/* select descriptor? (yes=1 or no=0) */
	int ser_inst_id;	/* Instance ID for Service UUID, default 0 */
	int char_inst_id;	/* Instance ID for Char UUID(eg. 0,1,2..) */
	int is_primary;		/* Is_primary value(eg:0,1) */
} ble_client_write_data;

typedef struct {
	int device_index;	/* 0 Device from XML database (already paired), 1 Device found in last discovery */
	unsigned char *ble_name; 	/* The ble device you want to connect */
	int client_num;
	int direct;		/* Direct connection:1, Background connection:0 */
} ble_client_connect_data;

/*******************************************************************************
 **
 ** Function         app_ble_client_find_free_space
 **
 ** Description      find free space for BLE client application
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_client_find_free_space(void);

/*******************************************************************************
 **
 ** Function         app_ble_client_display_attr
 **
 ** Description      Display BLE client's attribute list
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_client_display_attr(tAPP_BLE_CLIENT_DB_ELEMENT *p_blecl_db_elmt);

/*******************************************************************************
 **
 ** Function         app_ble_client_display
 **
 ** Description      Display BLE client
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_client_display(int detail);

/*******************************************************************************
 **
 ** Function         app_ble_client_find_reg_pending_index
 **
 ** Description      find registration pending index
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_client_find_reg_pending_index(void);

/*******************************************************************************
 **
 ** Function         app_ble_client_find_index_by_interface
 **
 ** Description      find BLE client index by interface
 **
 ** Parameters    if_num: interface number
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_client_find_index_by_interface(tBSA_BLE_IF if_num);

/*******************************************************************************
 **
 ** Function         app_ble_client_find_conn_id_by_interface
 **
 ** Description      find BLE client conn index by interface
 **
 ** Parameters    if_num: interface number
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_client_find_conn_id_by_interface(tBSA_BLE_IF if_num);

/*******************************************************************************
 **
 ** Function         app_ble_client_find_index_by_conn_id
 **
 ** Description      find BLE client index by connection ID
 **
 ** Parameters       conn_id: Connection ID
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_client_find_index_by_conn_id(UINT16 conn_id);

/*
 * BLE Client functions
 */
/*******************************************************************************
 **
 ** Function        app_ble_client_register
 **
 ** Description     This is the ble client register command
 **
 ** Parameters      uuid: uuid number of application
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_register(UINT16 uuid);

/*******************************************************************************
 **
 ** Function        app_ble_client_service_search
 **
 ** Description     This is the ble search service to ble server
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_service_search(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read
 **
 ** Description     This is the read function to read a characteristec or characteristic descriptor from BLE server
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read(ble_client_read_data *client_data);

/*******************************************************************************
 **
 ** Function        app_ble_client_write
 **
 ** Description     This is the write function to write a characteristic or characteristic discriptor to BLE server.
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_write(ble_client_write_data *cl_write_data);

/*******************************************************************************
 **
 ** Function        app_ble_client_register_notification
 **
 ** Description     This is the register function to receive a notification
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_register_notification(void);


/*******************************************************************************
 **
 ** Function        app_ble_client_open
 **
 ** Description     This is the ble open connection to ble server
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_open(ble_client_connect_data *cl_connect_data);

/*******************************************************************************
 **
 ** Function        app_ble_client_close
 **
 ** Description     This is the ble close connection
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_close(int client_num);

/*******************************************************************************
 **
 ** Function        app_mgr_sec_unpair_le
 **
 ** Description     Unpair LE device
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_mgr_sec_unpair_le(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_deregister
 **
 ** Description     This is the ble deregister app
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_deregister(int client_num);

/*******************************************************************************
 **
 ** Function        app_ble_client_deregister_all
 **
 ** Description     Deregister all apps
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_deregister_all(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_deregister_notification
 **
 ** Description     This is the deregister function for a notification
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_deregister_notification(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_load_attr
 **
 ** Description     This is the cache load function
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_load_attr(tAPP_BLE_CLIENT_DB_ELEMENT *p_blecl_db_elmt,
                             BD_ADDR bd_addr, UINT16 conn_id);

/*******************************************************************************
**
** Function         app_ble_client_profile_cback
**
** Description      BLE Client Profile callback.
**
** Returns          void
**
*******************************************************************************/
void app_ble_client_profile_cback(tBSA_BLE_EVT event,  tBSA_BLE_MSG *p_data);

/*******************************************************************************
 **
 ** Function        app_ble_client_service_search_execute
 **
 ** Description     This is the ble search service to ble server
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_service_search_execute(UINT16 service);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_battery_level
 **
 ** Description     Read BLE Device battery level
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_battery_level(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_mfr_name
 **
 ** Description     Read BLE Device manufacturer name
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_mfr_name(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_model_number
 **
 ** Description     Read BLE Device model number
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_model_number(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_serial_number
 **
 ** Description     Read BLE Device serial number
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_serial_number(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_hardware_revision
 **
 ** Description     Read BLE Device hardware revision
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_hardware_revision(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_firmware_revision
 **
 ** Description     Read BLE Device firmware revision
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_firmware_revision(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_software_revision
 **
 ** Description     Read BLE Device software revision
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_software_revision(void);

/*******************************************************************************
 **
 ** Function        app_ble_client_read_system_id
 **
 ** Description     Read BLE Device system ID
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_client_read_system_id(void);

/*******************************************************************************
**
** Function         app_ble_client_handle_read_evt
**
** Description      Function to handle callback for read events
**
** Returns          void
**
*******************************************************************************/
void app_ble_client_handle_read_evt(tBSA_BLE_CL_READ_MSG *cli_read);

/*******************************************************************************
**
** Function         app_ble_client_handle_indication
**
** Description      Function to handle callback for indications
**
** Returns          void
**
*******************************************************************************/
void app_ble_client_handle_indication(tBSA_BLE_CL_NOTIF_MSG *cli_notif);

/*******************************************************************************
**
** Function         app_ble_client_handle_notification
**
** Description      Function to handle callback for notification events
**
** Returns          void
**
*******************************************************************************/
void app_ble_client_handle_notification(tBSA_BLE_CL_NOTIF_MSG *cli_notif);

/* ---------------- ble client ------------------------------- */

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_client_register
 **
 ** Description     This is the ble client register command
 **
 ** Parameters      UINT16 uuid
 **
 ** Returns         status: client_num if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_client_register(UINT16 uuid);

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_client_deregister
 **
 ** Description     This is the ble deregister app
 **
 ** Parameters      int client_num
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_client_deregister(int client_num);

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_ble_start_regular
 **
 ** Description      Start BLE Device discovery
 **
 ** Returns          0 if success, -1 if failed
 **
 *******************************************************************************/
int mozart_bluetooth_ble_start_regular();

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_client_read
 **
 ** Description     This is the read function to read a characteristec or characteristic descriptor from BLE server
 **
 ** Parameters      struct ble_client_read_data
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
extern int mozart_bluetooth_ble_client_read(ble_client_read_data *client_data);

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_client_write
 **
 ** Description     This is the write function to write a characteristic or characteristic discriptor to BLE server.
 **
 ** Parameters      ble_client_write_data
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
extern int mozart_bluetooth_ble_client_write(ble_client_write_data *cl_write_data);

#endif
