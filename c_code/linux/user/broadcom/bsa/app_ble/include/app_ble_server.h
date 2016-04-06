/*****************************************************************************
**
**  Name:           app_ble_server.h
**
**  Description:    Bluetooth BLE include file
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef APP_BLE_SERVER_H
#define APP_BLE_SERVER_H

#include "bsa_api.h"
#include "app_ble.h"

typedef struct bsa_ble_create_service_data {
	int server_num;
	int attr_num;
	UINT16 service_uuid;
	UINT16  num_handle;
	BOOLEAN  is_primary;
} ble_create_service_data;

typedef struct bsa_ble_config_advertisement_data {
	UINT16	appearance_data;     /* Device appearance data, Eg:0x1122" */
	UINT16	number_of_services;  /* create services number */
	UINT16  services_uuid[BSA_DM_BLE_AD_UUID_MAX]; /*servies uuid value, eg:0xA108 */
	BOOLEAN	is_scan_rsp;  /* is the data scan response or adv data */
} ble_config_adv_data;

typedef struct bsa_ble_start_service_data {
	int server_num;
	int attr_num;
} ble_start_service_data;

typedef struct bsa_ble_add_character_data {
	int server_num;
	int srvc_attr_num;
	int char_attr_num;
	int is_descript;
	int attribute_permission;
	int characteristic_property;
	UINT16	char_uuid;
} ble_add_char_data;

/*******************************************************************************
 **
 ** Function         app_ble_server_find_free_space
 **
 ** Description      find free space for BLE server application
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_server_find_free_space(void);

/*******************************************************************************
 **
 ** Function         app_ble_server_display
 **
 ** Description      display BLE server
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_server_display(void);

/*******************************************************************************
 **
 ** Function         app_ble_server_find_reg_pending_index
 **
 ** Description      find registration pending index
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_server_find_reg_pending_index(void);

/*******************************************************************************
 **
 ** Function         app_ble_server_find_index_by_interface
 **
 ** Description      find BLE server index by interface
 **
 ** Parameters    if_num: interface number
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_server_find_index_by_interface(tBSA_BLE_IF if_num);


/*
 * BLE Server functions
 */
/*******************************************************************************
 **
 ** Function        app_ble_server_register
 **
 ** Description     Register server app
 **
 ** Parameters      uuid: uuid number of application
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_register(UINT16 uuid, tBSA_BLE_CBACK *p_cback);

/*******************************************************************************
 **
 ** Function        app_ble_server_deregister
 **
 ** Description     Deregister server app
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_deregister(int num);

/*******************************************************************************
 **
 ** Function        app_ble_server_create_service
 **
 ** Description     create service
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_create_service(ble_create_service_data *ble_create_service_data);

/*******************************************************************************
 **
 ** Function        app_ble_server_add_char
 **
 ** Description     Add charcter to service
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_add_char(ble_add_char_data *ble_add_char_data);

/*******************************************************************************
 **
 ** Function        app_ble_server_start_service
 **
 ** Description     Start Service
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_start_service(ble_start_service_data * ble_start_service_data);

/*******************************************************************************
 **
 ** Function        app_ble_server_send_indication
 **
 ** Description     Send indication to client
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_send_indication(void);

/*******************************************************************************
**
** Function         app_ble_server_profile_cback
**
** Description      BLE Server Profile callback.
**
** Returns          void
**
*******************************************************************************/
void app_ble_server_profile_cback(tBSA_BLE_EVT event,  tBSA_BLE_MSG *p_data);

/*******************************************************************************
 **
 ** Function        app_ble_server_open
 **
 ** Description     This is the ble open connection to ble client
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_open(void);

/*******************************************************************************
 **
 ** Function        app_ble_server_close
 **
 ** Description     This is the ble close connection
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_close(void);

#endif
