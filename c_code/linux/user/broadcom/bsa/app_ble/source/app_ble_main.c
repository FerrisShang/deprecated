/*****************************************************************************
 **
 **  Name:           app_ble_main.c
 **
 **  Description:    Bluetooth BLE Main application
 **
 **  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/
#include <stdlib.h>

#include "bsa_disc_api.h"
#include "app_ble.h"
#include "app_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_dm.h"
#include "app_utils.h"
#include "app_ble_client.h"
#include "app_ble_server.h"
#if defined(APP_BLE_OTA_FW_DL_INCLUDED) && (APP_BLE_OTA_FW_DL_INCLUDED == TRUE)
#include "app_ble_client_otafwdl.h"
#endif

/* BLE Menu items */
enum
{
	APP_BLE_MENU_ABORT_DISC = 1,
	APP_BLE_MENU_DISCOVERY,
	APP_BLE_MENU_REMOVE,
	APP_BLE_MENU_CONFIG_BLE_BG_CONN,
	APP_BLE_MENU_CONFIG_BLE_SCAN_PARAM,
	APP_BLE_MENU_CONFIG_BLE_CONN_PARAM,
	APP_BLE_MENU_CONFIG_BLE_ADV_PARAM,
	APP_BLE_MENU_WAKE_ON_BLE,

	APP_BLECL_MENU_REGISTER,
	APP_BLECL_MENU_DEREGISTER,
	APP_BLECL_MENU_OPEN,
	APP_BLECL_MENU_CLOSE,
	APP_BLECL_MENU_READ,
	APP_BLECL_MENU_WRITE,
	APP_BLECL_MENU_SERVICE_DISC,
	APP_BLECL_MENU_REG_FOR_NOTI,
	APP_BLECL_MENU_DEREG_FOR_NOTI,
	APP_BLECL_MENU_DISPLAY_CLIENT,
	APP_BLECL_MENU_SEARCH_DEVICE_INFORMATION_SERVICE,
	APP_BLECL_MENU_READ_MFR_NAME,
	APP_BLECL_MENU_READ_MODEL_NUMBER,
	APP_BLECL_MENU_READ_SERIAL_NUMBER,
	APP_BLECL_MENU_READ_HARDWARE_REVISION,
	APP_BLECL_MENU_READ_FIRMWARE_REVISION,
	APP_BLECL_MENU_READ_SOFTWARE_REVISION,
	APP_BLECL_MENU_READ_SYSTEM_ID,
	APP_BLECL_MENU_SEARCH_BATTERY_SERVICE,
	APP_BLECL_MENU_READ_BATTERY_LEVEL,
#if defined(APP_BLE_OTA_FW_DL_INCLUDED) && (APP_BLE_OTA_FW_DL_INCLUDED == TRUE)
	APP_BLECL_MENU_FW_UPGRADE,
#endif
	APP_BLESE_MENU_REGISTER,
	APP_BLESE_MENU_DEREGISTER,
	APP_BLESE_MENU_OPEN,
	APP_BLESE_MENU_CLOSE,
	APP_BLESE_MENU_CREATE_SERVICE,
	APP_BLESE_MENU_ADD_CHAR,
	APP_BLESE_MENU_START_SERVICE,
	APP_BLESE_MENU_CONFIG_BLE_ADV_DATA,
	APP_BLESE_MENU_DISPLAY_SERVER,
	APP_BLESE_MENU_SEND_IND,

	APP_BLE_MENU_QUIT = 99
};

/*
 * Global Variables
 */


/*
 * Local functions
 */


/*******************************************************************************
 **
 ** Function         app_display_ble_menu
 **
 ** Description      This is the BLE menu
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_display_menu (void)
{
	APP_INFO1("\t%d => Abort Discovery", APP_BLE_MENU_ABORT_DISC);
	APP_INFO1("\t%d => Start BLE Discovery", APP_BLE_MENU_DISCOVERY);
	APP_INFO1("\t%d => Remove BLE device", APP_BLE_MENU_REMOVE);
	APP_INFO1("\t%d => Configure BLE Background Connection Type", APP_BLE_MENU_CONFIG_BLE_BG_CONN);
	APP_INFO1("\t%d => Configure BLE Scan Parameter",APP_BLE_MENU_CONFIG_BLE_SCAN_PARAM);
	APP_INFO1("\t%d => Configure BLE Connection Parameter",APP_BLE_MENU_CONFIG_BLE_CONN_PARAM);
	APP_INFO1("\t%d => Configure BLE Advertisement Parameters",APP_BLE_MENU_CONFIG_BLE_ADV_PARAM);
	APP_INFO1("\t%d => Configure for Wake on BLE",APP_BLE_MENU_WAKE_ON_BLE);

	APP_INFO0("\t**** Bluetooth Low Energy Client menu ****");
	APP_INFO1("\t%d => Register client app", APP_BLECL_MENU_REGISTER);
	APP_INFO1("\t%d => Deregister Client app", APP_BLECL_MENU_DEREGISTER);
	APP_INFO1("\t%d => Connect to Server", APP_BLECL_MENU_OPEN);
	APP_INFO1("\t%d => Close Connection", APP_BLECL_MENU_CLOSE);
	APP_INFO1("\t%d => Read information", APP_BLECL_MENU_READ);
	APP_INFO1("\t%d => Write information", APP_BLECL_MENU_WRITE);
	APP_INFO1("\t%d => Service Discovery", APP_BLECL_MENU_SERVICE_DISC);
	APP_INFO1("\t%d => Register Notification", APP_BLECL_MENU_REG_FOR_NOTI);
	APP_INFO1("\t%d => Deregister Notification", APP_BLECL_MENU_DEREG_FOR_NOTI);
	APP_INFO1("\t%d => Display Clients", APP_BLECL_MENU_DISPLAY_CLIENT);
	APP_INFO1("\t%d => Search Device Information Service", APP_BLECL_MENU_SEARCH_DEVICE_INFORMATION_SERVICE);
	APP_INFO1("\t%d => Read Manufacturer Name", APP_BLECL_MENU_READ_MFR_NAME);
	APP_INFO1("\t%d => Read Model Number", APP_BLECL_MENU_READ_MODEL_NUMBER);
	APP_INFO1("\t%d => Read Serial Number", APP_BLECL_MENU_READ_SERIAL_NUMBER);
	APP_INFO1("\t%d => Read Hardware Revision", APP_BLECL_MENU_READ_HARDWARE_REVISION);
	APP_INFO1("\t%d => Read Firmware Revision", APP_BLECL_MENU_READ_FIRMWARE_REVISION);
	APP_INFO1("\t%d => Read Software Revision", APP_BLECL_MENU_READ_SOFTWARE_REVISION);
	APP_INFO1("\t%d => Read System ID", APP_BLECL_MENU_READ_SYSTEM_ID);
	APP_INFO1("\t%d => Search Battery Service", APP_BLECL_MENU_SEARCH_BATTERY_SERVICE);
	APP_INFO1("\t%d => Read Battery Level", APP_BLECL_MENU_READ_BATTERY_LEVEL);
#if defined(APP_BLE_OTA_FW_DL_INCLUDED) && (APP_BLE_OTA_FW_DL_INCLUDED == TRUE)
	APP_INFO1("\t%d => Upgrade FW by LE",APP_BLECL_MENU_FW_UPGRADE);
#endif
	APP_INFO0("\t**** Bluetooth Low Energy Server menu ****");
	APP_INFO1("\t%d => Register server app", APP_BLESE_MENU_REGISTER);
	APP_INFO1("\t%d => Deregister server app", APP_BLESE_MENU_DEREGISTER);
	APP_INFO1("\t%d => Connect to client", APP_BLESE_MENU_OPEN);
	APP_INFO1("\t%d => Close connection", APP_BLESE_MENU_CLOSE);
	APP_INFO1("\t%d => Create service", APP_BLESE_MENU_CREATE_SERVICE);
	APP_INFO1("\t%d => Add character", APP_BLESE_MENU_ADD_CHAR);
	APP_INFO1("\t%d => Start service", APP_BLESE_MENU_START_SERVICE);
	APP_INFO1("\t%d => Configure BLE Advertisement data",APP_BLESE_MENU_CONFIG_BLE_ADV_DATA);
	APP_INFO1("\t%d => Display Servers", APP_BLESE_MENU_DISPLAY_SERVER);
	APP_INFO1("\t%d => Send indication", APP_BLESE_MENU_SEND_IND);

	APP_INFO1("\t%d => QUIT", APP_BLE_MENU_QUIT);
}

/*
 * BLE Server functions
 */
/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_server_register
 **
 ** Description     Register server app
 **
 ** Parameters      uuid
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_server_register(UINT16 uuid)
{
	return app_ble_server_register(uuid, NULL);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_server_deregister
 **
 ** Description     Deregister server app
 **
 ** Parameters      server_num: server number
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_server_deregister(int server_num)
{
	return app_ble_server_deregister(server_num);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_create_service
 **
 ** Description     create GATT service
 **
 ** Parameters      struct ble_service_info
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_create_service(ble_create_service_data * ble_create_service_data)
{
	return app_ble_server_create_service(ble_create_service_data);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_add_character
 **
 ** Description     Add character to service
 **
 ** Parameters      struct bsa_ble_add_character
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_add_character(ble_add_char_data *ble_add_char_data)
{
	return app_ble_server_add_char(ble_add_char_data);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_configure_advertisement_data
 **
 ** Description     start BLE advertising
 **
 ** Parameters      struct bsa_ble_config_advertisement_data
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_configure_adv_data(ble_config_adv_data *ble_config_adv_data)
{
	int i = 0;
	tBSA_DM_BLE_ADV_CONFIG adv_conf;
	UINT8 app_ble_adv_value[APP_BLE_ADV_VALUE_LEN] = {0x2b, 0x1a, 0xaa, 0xbb, 0xcc, 0xdd}; /*First 2 byte is Company Identifier Eg: 0x1a2b refers to Bluetooth ORG, followed by 4bytes of data*/

	/* This is just sample code to show how BLE Adv data can be sent from application */
	/*Adv.Data should be < 31bytes including Manufacturer data,Device Name, Appearance data, Services Info,etc.. */
	/* We are not receving all fields from user to reduce the complexity */
	memset(&adv_conf, 0, sizeof(tBSA_DM_BLE_ADV_CONFIG));
	adv_conf.appearance_data = ble_config_adv_data->appearance_data;
	adv_conf.num_service = ble_config_adv_data->number_of_services;
	adv_conf.is_scan_rsp = ble_config_adv_data->is_scan_rsp;
	printf("adv_data: appearance_data = 0x%x, num_service = %d, is_scan_rsp = %d\n",
	     adv_conf.appearance_data, adv_conf.num_service, adv_conf.is_scan_rsp);

	/* start advertising */
	adv_conf.len = APP_BLE_ADV_VALUE_LEN;
	adv_conf.flag = BSA_DM_BLE_ADV_FLAG_MASK;
	memcpy(adv_conf.p_val, app_ble_adv_value, APP_BLE_ADV_VALUE_LEN);
	/* All the masks/fields that are set will be advertised*/
	adv_conf.adv_data_mask = BSA_DM_BLE_AD_BIT_FLAGS|BSA_DM_BLE_AD_BIT_SERVICE|BSA_DM_BLE_AD_BIT_APPEARANCE|BSA_DM_BLE_AD_BIT_MANU|BSA_DM_BLE_AD_BIT_DEV_NAME;
	for(i = 0; i < adv_conf.num_service; i++)
	{

		adv_conf.uuid_val[i]= ble_config_adv_data->services_uuid[i];
		printf("adv_data: adv_conf.uuid_val[%d] = 0x%x\n", i, adv_conf.uuid_val[i]);
	}

	return app_dm_set_ble_adv_data(&adv_conf);
}

/*******************************************************************************
 **
 ** Function       mozart_bluetooth_ble_start_service
 **
 ** Description     Start Service
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_start_service(ble_start_service_data *ble_start_service_data)
{
	return app_ble_server_start_service(ble_start_service_data);
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_ble_server_display
 **
 ** Description      display BLE server
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
void mozart_bluetooth_ble_server_display()
{
	app_ble_server_display();
}

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
int mozart_bluetooth_ble_client_register(UINT16 uuid)
{
	return app_ble_client_register(uuid);
}

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
int mozart_bluetooth_ble_client_deregister(int client_num)
{
	return app_ble_client_deregister(client_num);
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_ble_start_regular
 **
 ** Description      Start BLE Device discovery
 **
 ** Returns          0 if success, -1 if failed
 **
 *******************************************************************************/
int mozart_bluetooth_ble_start_regular()
{
	return app_disc_start_ble_regular(NULL);
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_find_disc_device_name
 **
 ** Description      Find a device in the list of discovered devices
 **
 ** Parameters       index_device
 **
 ** Returns          index_device if successful, -1 is failed.
 **
 *******************************************************************************/
int mozart_bluetooth_find_disc_device_name(unsigned char *name)
{
	return app_disc_find_device_name(name);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_client_connect_server
 **
 ** Description     This is the ble open connection to ble server
 **
 ** Parameters      struct ble_client_connect_data
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_client_connect_server(ble_client_connect_data *cl_connect_data)
{
	return app_ble_client_open(cl_connect_data);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_client_disconnect_server
 **
 ** Description     This is the ble close connection
 **
 ** Parameters      client_num
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_client_disconnect_server(int client_num)
{
	return app_ble_client_close(client_num);
}
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
int mozart_bluetooth_ble_client_read(ble_client_read_data *client_data)
{
	return app_ble_client_read(client_data);
}

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
int mozart_bluetooth_ble_client_write(ble_client_write_data *cl_write_data)
{
	return app_ble_client_write(cl_write_data);
}
/* ----------------- ble Client ------------------------------ */
/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_start
 **
 ** Description     start BSA BLE Function
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_start()
{
	int status;

	/* Initialize BLE application */
	status = app_ble_init();
	if (status < 0)
	{
		APP_ERROR0("Couldn't Initialize BLE app, exiting");
		return -1;
	}
	/* Start BLE application */
	status = app_ble_start();
	if (status < 0)
	{
		APP_ERROR0("Couldn't Start BLE app, exiting");
		return -1;
	}

	return 0;
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_close
 **
 ** Description     Close BSA BLE Function
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_close()
{
	/* Exit BLE mode */
	return app_ble_exit();
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_get_server_char_value
 **
 ** Description      Get BLE character value
 **
 ** Parameter        server_num: service num. character_num: character num. value: character value.
 **
 ** Returns          character value length if service enabled, -1 if service not enabled
 *******************************************************************************/
int mozart_bluetooth_get_server_char_value(int server_num, int character_num, UINT8 *value)
{
	int value_len = app_ble_cb.ble_server[server_num].attr[character_num].value_len;

	if (app_ble_cb.ble_server[server_num].enabled) {
			memcpy(value, app_ble_cb.ble_server[server_num].attr[character_num].value, value_len);
			return value_len;
	} else {
		printf("app_ble_cb.ble_server[%d].enabled = %d, not enabled!\n", server_num, app_ble_cb.ble_server[server_num].enabled);
		return -1;
	}

}
