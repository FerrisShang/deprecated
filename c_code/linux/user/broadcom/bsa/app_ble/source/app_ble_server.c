/*****************************************************************************
 **
 **  Name:           app_ble_server.c
 **
 **  Description:    Bluetooth BLE Server general application
 **
 **  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#include "app_ble.h"
#include "app_xml_utils.h"
#include "app_utils.h"
#include "app_mgt.h"
#include "app_disc.h"
#include "app_dm.h"

#include "app_ble_server.h"
#include "event_interface.h"

mozart_event mozart_ble_se_create_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "se_create",
		},
	},
	.type = EVENT_MISC,
};

mozart_event mozart_ble_se_addchar_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "se_addchar",
		},
	},
	.type = EVENT_MISC,
};

mozart_event mozart_ble_se_start_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "se_start",
		},
	},
	.type = EVENT_MISC,
};

mozart_event mozart_ble_se_read_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "se_r_complete",
		},
	},
	.type = EVENT_MISC,
};

mozart_event mozart_ble_se_write_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "se_w_complete",
		},
	},
	.type = EVENT_MISC,
};
/*
 * Global Variables
 */


/*
 * Local functions
 */


/*
 * BLE common functions
 */


/*******************************************************************************
 **
 ** Function         app_ble_server_find_free_server
 **
 ** Description      find free server for BLE server application
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_server_find_free_server(void)
{
	int index;

	for (index = 0; index < BSA_BLE_SERVER_MAX; index++)
	{
		if (!app_ble_cb.ble_server[index].enabled)
		{
			return index;
		}
	}
	return -1;
}

/*******************************************************************************
 **
 ** Function         app_ble_server_find_free_attr
 **
 ** Description      find free attr for BLE server application
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_ble_server_find_free_attr(UINT16 server)
{
	int index;

	for (index = 0; index < BSA_BLE_ATTRIBUTE_MAX; index++)
	{
		if (!app_ble_cb.ble_server[server].attr[index].attr_UUID.uu.uuid16)
		{
			return index;
		}
	}
	return -1;
}

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
void app_ble_server_display(void)
{
	int index, attr_num;

	APP_INFO0("*************** BLE SERVER LIST *****************");
	for (index = 0; index < BSA_BLE_SERVER_MAX; index++)
	{
		if (app_ble_cb.ble_server[index].enabled)
		{
			APP_INFO1("%d:BLE Server server_if:%d", index,
					app_ble_cb.ble_server[index].server_if);
			for (attr_num = 0; attr_num < BSA_BLE_ATTRIBUTE_MAX ; attr_num++)
			{
				if (app_ble_cb.ble_server[index].attr[attr_num].attr_UUID.uu.uuid16)
				{
					if ((app_ble_cb.ble_server[index].attr[attr_num].attr_type == BSA_GATTC_ATTR_TYPE_SRVC) ||
							(app_ble_cb.ble_server[index].attr[attr_num].attr_type == BSA_GATTC_ATTR_TYPE_INCL_SRVC))
					{
						APP_INFO1("\t attr_num:%d:uuid:0x%04x, is_pri:%d, service_id:%d attr_id:%d",
								attr_num,
								app_ble_cb.ble_server[index].attr[attr_num].attr_UUID.uu.uuid16,
								app_ble_cb.ble_server[index].attr[attr_num].is_pri,
								app_ble_cb.ble_server[index].attr[attr_num].service_id,
								app_ble_cb.ble_server[index].attr[attr_num].attr_id);
					}
					else
					{
						APP_INFO1("\t\t attr_num:%d:uuid:0x%04x, is_pri:%d, service_id:%d attr_id:%d",
								attr_num,
								app_ble_cb.ble_server[index].attr[attr_num].attr_UUID.uu.uuid16,
								app_ble_cb.ble_server[index].attr[attr_num].is_pri,
								app_ble_cb.ble_server[index].attr[attr_num].service_id,
								app_ble_cb.ble_server[index].attr[attr_num].attr_id);
					}
				}
			}
		}
	}
	APP_INFO0("*************** BLE SERVER LIST END *************");
}

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
int app_ble_server_find_reg_pending_index(void)
{
	int index;

	for (index = 0; index < BSA_BLE_SERVER_MAX; index++)
	{
		if ((app_ble_cb.ble_server[index].enabled) &&
				(app_ble_cb.ble_server[index].server_if == BSA_BLE_INVALID_IF))
		{
			return index;
		}
	}
	return -1;
}

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
int app_ble_server_find_index_by_interface(tBSA_BLE_IF if_num)
{
	int index;

	for (index = 0; index < BSA_BLE_SERVER_MAX; index++)
	{
		if (app_ble_cb.ble_server[index].server_if == if_num)
		{
			return index;
		}
	}
	return -1;
}


/*
 * BLE Server functions
 */
/*******************************************************************************
 **
 ** Function        app_ble_server_register
 **
 ** Description     Register server app
 **
 ** Parameters      None
 **
 ** Returns         status: server_num if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_register(UINT16 uuid, tBSA_BLE_CBACK *p_cback)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_REGISTER ble_register_param;
	UINT16 choice;
	int server_num = 0;

	server_num = app_ble_server_find_free_server();
	if (server_num < 0)
	{
		APP_ERROR0("No more spaces!!");
		return -1;
	}
	printf("server_num = %d\n", server_num);
	if (!uuid)
	{
		printf("Warning: uuid is 0, use default UUID: %d\n", APP_BLE_MAIN_DEFAULT_APPL_UUID);
		choice = APP_BLE_MAIN_DEFAULT_APPL_UUID;
	}
	else
	{
		choice = uuid;
	}

	status = BSA_BleSeAppRegisterInit(&ble_register_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeAppRegisterInit failed status = %d", status);
		return -1;
	}

	ble_register_param.uuid.len = 2;
	ble_register_param.uuid.uu.uuid16 = choice;
	if (p_cback == NULL)
	{
		ble_register_param.p_cback = app_ble_server_profile_cback;
	}
	else
	{
		ble_register_param.p_cback = p_cback;
	}

	status = BSA_BleSeAppRegister(&ble_register_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeAppRegister failed status = %d", status);
		return -1;
	}
	app_ble_cb.ble_server[server_num].enabled = TRUE;
	app_ble_cb.ble_server[server_num].server_if = ble_register_param.server_if;
	APP_INFO1("enabled:%d, server_if:%d", app_ble_cb.ble_server[server_num].enabled,
			app_ble_cb.ble_server[server_num].server_if);
	return server_num;
}

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
int app_ble_server_deregister(int num)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_DEREGISTER ble_deregister_param;

	APP_INFO0("Bluetooth BLE deregister menu:");
	APP_INFO0("Select Server:");
	app_ble_server_display();
	if ((num < 0) || (num >= BSA_BLE_SERVER_MAX))
	{
		APP_ERROR1("Wrong server number! = %d", num);
		return -1;
	}
	if (app_ble_cb.ble_server[num].enabled != TRUE)
	{
		APP_ERROR1("Server was not registered! = %d", num);
		return -1;
	}

	status = BSA_BleSeAppDeregisterInit(&ble_deregister_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeAppDeregisterInit failed status = %d", status);
		return -1;
	}

	ble_deregister_param.server_if = app_ble_cb.ble_server[num].server_if;

	status = BSA_BleSeAppDeregister(&ble_deregister_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeAppDeregister failed status = %d", status);
		return -1;
	}

	return 0;
}

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
int app_ble_server_create_service(ble_create_service_data *ble_create_service_data)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_CREATE ble_create_param;
	UINT16 service;
	UINT16  num_handle;
	BOOLEAN is_primary;
	int server_num, attr_num;

	server_num = ble_create_service_data->server_num;
	service = ble_create_service_data->service_uuid;
	num_handle = ble_create_service_data->num_handle;
	is_primary = ble_create_service_data->is_primary;

	printf("create service: server_num = %d, service = 0x%x, num_handle = %d, is_primary = %d\n",
			server_num, service, num_handle, is_primary);
	if ((server_num < 0) || (server_num >= BSA_BLE_SERVER_MAX))
	{
		APP_ERROR1("Wrong server number! = %d", server_num);
		return -1;
	}
	if (app_ble_cb.ble_server[server_num].enabled != TRUE)
	{
		APP_ERROR1("Server was not enabled! = %d", server_num);
		return -1;
	}

	if (!service)
	{
		APP_ERROR1("wrong value = %d", service);
		return -1;
	}

	if (!num_handle)
	{
		APP_ERROR1("wrong value = %d", num_handle);
		return -1;
	}
	if (!(is_primary == 0) && !(is_primary == 1))
	{
		APP_ERROR1("wrong value = %d", is_primary);
		return -1;
	}
	attr_num = app_ble_server_find_free_attr(server_num);
	ble_create_service_data->attr_num = attr_num;
	printf("attr_num = %d\n", attr_num);
	if (attr_num < 0)
	{
		APP_ERROR1("Wrong attr number! = %d", attr_num);
		return -1;
	}
	status = BSA_BleSeCreateServiceInit(&ble_create_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeCreateServiceInit failed status = %d", status);
		return -1;
	}

	ble_create_param.service_uuid.uu.uuid16 = service;
	ble_create_param.service_uuid.len = 2;
	ble_create_param.server_if = app_ble_cb.ble_server[server_num].server_if;
	ble_create_param.num_handle = num_handle;
	if (is_primary != 0)
	{
		ble_create_param.is_primary = TRUE;
	}
	else
	{
		ble_create_param.is_primary = FALSE;
	}
	app_ble_cb.ble_server[server_num].attr[attr_num].wait_flag = TRUE;

	status = BSA_BleSeCreateService(&ble_create_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeCreateService failed status = %d", status);
		app_ble_cb.ble_server[server_num].attr[attr_num].wait_flag = FALSE;
		return -1;
	}

	/* store information on control block */
	app_ble_cb.ble_server[server_num].attr[attr_num].attr_UUID.len = 2;
	app_ble_cb.ble_server[server_num].attr[attr_num].attr_UUID.uu.uuid16 = service;
	app_ble_cb.ble_server[server_num].attr[attr_num].is_pri = ble_create_param.is_primary;
	app_ble_cb.ble_server[server_num].attr[attr_num].attr_type = BSA_GATTC_ATTR_TYPE_SRVC;

	return 0;
}
/*******************************************************************************
 **
 ** Function        app_ble_server_add_char
 **
 ** Description     Add character to service
 **
 ** Parameters      struct bsa_ble_add_character
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_server_add_char(ble_add_char_data *ble_add_char_data)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_ADDCHAR ble_addchar_param;
	UINT16 char_uuid;
	int server_num, srvc_attr_num, char_attr_num;
	int attribute_permission;
	int characteristic_property = 0;
	int is_descript;

	server_num = ble_add_char_data->server_num;
	srvc_attr_num = ble_add_char_data->srvc_attr_num;
	char_uuid = ble_add_char_data->char_uuid;
	is_descript = ble_add_char_data->is_descript;
	attribute_permission = ble_add_char_data->attribute_permission;
	characteristic_property = ble_add_char_data->characteristic_property;

	printf("add_char: server_num = %d, srvc_attr_num = %d, char_uuid = 0x%x, is_descr = %d, perm = 0x%x, characteristic_property = 0x%x\n",
			server_num, srvc_attr_num, char_uuid, ble_addchar_param.is_descr,
			ble_addchar_param.perm, characteristic_property);

	if ((server_num < 0) || (server_num >= BSA_BLE_SERVER_MAX))
	{
		APP_ERROR1("Wrong server number! = %d", server_num);
		return -1;
	}
	if (app_ble_cb.ble_server[server_num].enabled != TRUE)
	{
		APP_ERROR1("Server was not enabled! = %d", server_num);
		return -1;
	}

	if(srvc_attr_num < 0)
	{
		APP_ERROR0("app_ble_server_add_char : Invalid srvc_attr_num entered");
		return -1;
	}

	char_attr_num = app_ble_server_find_free_attr(server_num);
	ble_add_char_data->char_attr_num = char_attr_num;
	status = BSA_BleSeAddCharInit(&ble_addchar_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeAddCharInit failed status = %d", status);
		return -1;
	}
	ble_addchar_param.service_id = app_ble_cb.ble_server[server_num].attr[srvc_attr_num].service_id;
	ble_addchar_param.char_uuid.len = 2;
	ble_addchar_param.char_uuid.uu.uuid16 = char_uuid;

	if(is_descript)
	{
		ble_addchar_param.is_descr = TRUE;

		APP_INFO0("Attribute Permissions[Eg: Read-0x1, Write-0x10, Read|Write-0x11]");
		ble_addchar_param.perm = attribute_permission;
	}
	else
	{
		ble_addchar_param.is_descr = FALSE;

		APP_INFO0("Attribute Permissions[Eg: Read-0x1, Write-0x10, Read|Write-0x11]");
		ble_addchar_param.perm = attribute_permission;

		APP_INFO0("Characterisic Properties Eg: WRITE-0x08, READ-0x02, Notify-0x10, Indicate-0x20");
		APP_INFO0("Eg: For READ|WRITE|NOTIFY|INDICATE enter 0x3A");
		ble_addchar_param.property = characteristic_property;
	}

	/* save all information */
	app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_UUID.len = 2;
	app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_UUID.uu.uuid16 = char_uuid;
	app_ble_cb.ble_server[server_num].attr[char_attr_num].prop = characteristic_property;
	app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
	app_ble_cb.ble_server[server_num].attr[char_attr_num].wait_flag = TRUE;
	memset(app_ble_cb.ble_server[server_num].attr[char_attr_num].value, 0, BSA_BLE_MAX_ATTR_LEN);
	app_ble_cb.ble_server[server_num].attr[char_attr_num].value_len = 0;

	status = BSA_BleSeAddChar(&ble_addchar_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeAddChar failed status = %d", status);
		return -1;
	}

	return 0;
}
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
int app_ble_server_start_service(ble_start_service_data * ble_start_service_data)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_START ble_start_param;
	int num, attr_num;

	num = ble_start_service_data->server_num;
	attr_num = ble_start_service_data->attr_num;
	printf("server_num = %d, attr_num = %d\n", num, attr_num);

	if ((num < 0) || (num >= BSA_BLE_SERVER_MAX))
	{
		APP_ERROR1("Wrong server number! = %d", num);
		return -1;
	}
	if (app_ble_cb.ble_server[num].enabled != TRUE)
	{
		APP_ERROR1("Server was not enabled! = %d", num);
		return -1;
	}

	if(attr_num < 0)
	{
		APP_ERROR0("app_ble_server_start_service : Invalid attr_num entered");
		return -1;
	}

	status = BSA_BleSeStartServiceInit(&ble_start_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeStartServiceInit failed status = %d", status);
		return -1;
	}

	ble_start_param.service_id = app_ble_cb.ble_server[num].attr[attr_num].service_id;
	ble_start_param.sup_transport = BSA_BLE_GATT_TRANSPORT_LE_BR_EDR;

	APP_INFO1("service_id:%d, num:%d", ble_start_param.service_id, num);

	status = BSA_BleSeStartService(&ble_start_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeStartService failed status = %d", status);
		return -1;
	}
	return 0;
}
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
int app_ble_server_send_indication(void)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_SENDIND ble_sendind_param;
	int num, length_of_data, index, attr_num;

	APP_INFO0("Select Server:");
	app_ble_server_display();
	num = app_get_choice("Select");
	if ((num < 0) || (num >= BSA_BLE_SERVER_MAX))
	{
		APP_ERROR1("Wrong server number! = %d", num);
		return -1;
	}
	if (app_ble_cb.ble_server[num].enabled != TRUE)
	{
		APP_ERROR1("Server was not enabled! = %d", num);
		return -1;
	}
	APP_INFO0("Select Service's attribute number :");
	if (-1 == (attr_num = app_get_choice("Select")))
		return -1;

	status = BSA_BleSeSendIndInit(&ble_sendind_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeSendIndInit failed status = %d", status);
		return -1;
	}

	ble_sendind_param.conn_id = app_ble_cb.ble_server[num].conn_id;
	ble_sendind_param.attr_id = app_ble_cb.ble_server[num].attr[attr_num].attr_id;

	length_of_data = app_get_choice("Enter length of data");
	ble_sendind_param.data_len = length_of_data;

	for (index = 0; index < length_of_data ; index++)
	{
		ble_sendind_param.value[index] = app_get_choice("Enter data in byte");
	}

	ble_sendind_param.need_confirm = FALSE;

	status = BSA_BleSeSendInd(&ble_sendind_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeSendInd failed status = %d", status);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_ble_server_profile_cback
 **
 ** Description      BLE Server Profile callback.
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_server_profile_cback(tBSA_BLE_EVT event,  tBSA_BLE_MSG *p_data)
{
	int i = 0;
	int err;
	int num, attr_index, index;
	tBSA_BLE_SE_SENDRSP send_server_resp;

	APP_DEBUG1("app_ble_server_profile_cback: event = %d ", event);
	switch (event)
	{
		case BSA_BLE_SE_DEREGISTER_EVT:
			printf("++++ BSA_BLE_SE_DEREGISTER_EVT!\n");
			APP_INFO1("server_if:%d status:%d",
					p_data->ser_deregister.server_if, p_data->ser_deregister.status);
			num = app_ble_server_find_index_by_interface(p_data->ser_deregister.server_if);
			if(num < 0)
			{
				APP_ERROR0("no deregister pending!!");
				break;
			}

			app_ble_cb.ble_server[num].server_if = BSA_BLE_INVALID_IF;
			app_ble_cb.ble_server[num].enabled = FALSE;
			for (attr_index = 0 ; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
			{
				memset(&app_ble_cb.ble_server[num].attr[attr_index], 0, sizeof(tAPP_BLE_ATTRIBUTE));
			}

			break;

		case BSA_BLE_SE_CREATE_EVT:
			printf("++++ BSA_BLE_SE_CREATE_EVT!\n");
			APP_INFO1("server_if:%d status:%d service_id:%d",
					p_data->ser_create.server_if, p_data->ser_create.status, p_data->ser_create.service_id);

			num = app_ble_server_find_index_by_interface(p_data->ser_create.server_if);
			printf("p_data->ser_create.server_if = %d\n", p_data->ser_create.server_if);
			/* search interface number */
			if(num < 0)
			{
				APP_ERROR0("no interface!!");
				break;
			}

			/* search attribute number */
			for (attr_index = 0 ; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
			{
				if (app_ble_cb.ble_server[num].attr[attr_index].wait_flag == TRUE)
				{
					APP_INFO1("if_num:%d, attr_num:%d", num, attr_index);
					if (p_data->ser_create.status == BSA_SUCCESS)
					{
						app_ble_cb.ble_server[num].attr[attr_index].service_id = p_data->ser_create.service_id;
						app_ble_cb.ble_server[num].attr[attr_index].wait_flag = FALSE;
						break;
					}
					else  /* if CREATE fail */
					{
						memset(&app_ble_cb.ble_server[num].attr[attr_index], 0, sizeof(tAPP_BLE_ATTRIBUTE));
						break;
					}
				}
			}
			if (attr_index >= BSA_BLE_ATTRIBUTE_MAX)
			{
				APP_ERROR0("no waiting!!");
				break;
			}
			err = mozart_event_send(mozart_ble_se_create_event);
			if (err < 0)
				printf("Send ble create event failure\n");

			app_ble_server_display();
			break;

		case BSA_BLE_SE_ADDCHAR_EVT:
			printf("++++ BSA_BLE_SE_ADDCHAR_EVT!\n");
			APP_INFO1("status: %d", p_data->ser_addchar.status);
			APP_INFO1("attr_id: 0x%x", p_data->ser_addchar.attr_id);

			num = app_ble_server_find_index_by_interface(p_data->ser_addchar.server_if);
			/* search interface number */
			if(num < 0)
			{
				APP_ERROR0("no interface!!");
				break;
			}

			for (attr_index = 0 ; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
			{
				if (app_ble_cb.ble_server[num].attr[attr_index].wait_flag == TRUE)
				{
					APP_INFO1("if_num:%d, attr_index:%d", num, attr_index);
					if (p_data->ser_addchar.status == BSA_SUCCESS)
					{
						app_ble_cb.ble_server[num].attr[attr_index].service_id = p_data->ser_addchar.service_id;
						app_ble_cb.ble_server[num].attr[attr_index].attr_id = p_data->ser_addchar.attr_id;
						app_ble_cb.ble_server[num].attr[attr_index].wait_flag = FALSE;
						break;
					}
					else  /* if CREATE fail */
					{
						memset(&app_ble_cb.ble_server[num].attr[attr_index], 0, sizeof(tAPP_BLE_ATTRIBUTE));
						break;
					}
				}
			}
			if (attr_index >= BSA_BLE_ATTRIBUTE_MAX)
			{
				APP_ERROR0("no waiting!!");
				break;
			}
			err = mozart_event_send(mozart_ble_se_addchar_event);
			if (err < 0)
				printf("Send ble create event failure\n");
			app_ble_server_display();
			break;

		case BSA_BLE_SE_START_EVT:
			printf("++++ BSA_BLE_SE_START_EVT!\n");
			APP_INFO1("status:%d", p_data->ser_start.status);

			err = mozart_event_send(mozart_ble_se_start_event);
			if (err < 0)
				printf("Send ble create event failure\n");
			break;

		case BSA_BLE_SE_READ_EVT:
			printf("++++ BSA_BLE_SE_READ_EVT!\n");
			APP_INFO1("status:%d", p_data->ser_read.status);
			BSA_BleSeSendRspInit(&send_server_resp);
			send_server_resp.conn_id = p_data->ser_read.conn_id;
			send_server_resp.trans_id = p_data->ser_read.trans_id;
			send_server_resp.status = p_data->ser_read.status;
			send_server_resp.handle = p_data->ser_read.handle;
			send_server_resp.offset = p_data->ser_read.offset;
			send_server_resp.len = 4;
			send_server_resp.auth_req = GATT_AUTH_REQ_NONE;

			for (index = 0; index < BSA_BLE_SERVER_MAX; index++)
			{
				if (app_ble_cb.ble_server[index].enabled)
				{
					APP_INFO1("%d:BLE Server server_if:%d", index,
							app_ble_cb.ble_server[index].server_if);
					for (attr_index = 0; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
					{
						if (app_ble_cb.ble_server[index].attr[attr_index].attr_UUID.uu.uuid16)
						{
							if (app_ble_cb.ble_server[index].attr[attr_index].attr_id == send_server_resp.handle) {
								printf("app_ble_cb.ble_server[%d].attr[%d].attr_id = %d\n",
										index, attr_index,
										app_ble_cb.ble_server[index].attr[attr_index].attr_id);
								printf("app_ble_cb.ble_server[%d].attr[%d].attr_UUID.uu.uuid16 = 0x%02x\n",
										index, attr_index,
										app_ble_cb.ble_server[index].attr[attr_index].attr_UUID.uu.uuid16);
								memcpy(send_server_resp.value, app_ble_cb.ble_server[index].attr[attr_index].value, BSA_BLE_MAX_ATTR_LEN);
								for (i = 0; i < send_server_resp.len; i++)
									printf("send_server_read.value = 0x%x\n",send_server_resp.value[i]);
								goto BleSeSendRSP;
							}
						}
					}
				}
			}
BleSeSendRSP:
			if (index >= BSA_BLE_SERVER_MAX) {
				printf("Error: not find Ble Server, please check!\n");
				break;
			}

			APP_INFO1("send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
			APP_INFO1("send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
			APP_INFO1("send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
			BSA_BleSeSendRsp(&send_server_resp);

			err = mozart_event_send(mozart_ble_se_read_event);
			if (err < 0)
				printf("Send ble read complete event failure\n");
			break;

		case BSA_BLE_SE_WRITE_EVT:
			printf("++++ BSA_BLE_SE_WRITE_EVT !\n");
			APP_INFO1("status:%d", p_data->ser_write.status);
			printf("p_data->ser_write.handle = %d\n", p_data->ser_write.handle);
			APP_DUMP("Write value", p_data->ser_write.value, p_data->ser_write.len);
			for (i = 0; i < p_data->ser_write.len; i++) {
				printf("ser_write.value = 0x%x\n", p_data->ser_write.value[i]);
			}
			APP_INFO1("trans_id:%d, conn_id:%d, handle:%d",
					p_data->ser_write.trans_id, p_data->ser_write.conn_id, p_data->ser_write.handle);
			for (index = 0; index < BSA_BLE_SERVER_MAX; index++)
			{
				if (app_ble_cb.ble_server[index].enabled)
				{
					APP_INFO1("%d:BLE Server server_if:%d", index,
							app_ble_cb.ble_server[index].server_if);
					for (attr_index = 0; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
					{
						if (app_ble_cb.ble_server[index].attr[attr_index].attr_UUID.uu.uuid16)
						{
							if (app_ble_cb.ble_server[index].attr[attr_index].attr_id == send_server_resp.handle) {
								printf("app_ble_cb.ble_server[%d].attr[%d].attr_id = %d\n",
										index, attr_index,
										app_ble_cb.ble_server[index].attr[attr_index].attr_id);
								printf("app_ble_cb.ble_server[%d].attr[%d].attr_UUID.uu.uuid16 = 0x%02x\n",
										index, attr_index,
										app_ble_cb.ble_server[index].attr[attr_index].attr_UUID.uu.uuid16);
								memcpy(app_ble_cb.ble_server[index].attr[attr_index].value, p_data->ser_write.value, BSA_BLE_MAX_ATTR_LEN);
								app_ble_cb.ble_server[index].attr[attr_index].value_len = p_data->ser_write.len;
								goto BLE_SE_WRITE_SendRSP;
							}
						}
					}
				}
			}
BLE_SE_WRITE_SendRSP:
			printf("index = %d\n", index);
			if (index >= BSA_BLE_SERVER_MAX) {
				printf("Error: not find Ble Server, please check!\n");
				break;
			}

			if (p_data->ser_write.need_rsp)
			{
				BSA_BleSeSendRspInit(&send_server_resp);
				send_server_resp.conn_id = p_data->ser_write.conn_id;
				send_server_resp.trans_id = p_data->ser_write.trans_id;
				send_server_resp.status = p_data->ser_write.status;
				send_server_resp.handle = p_data->ser_write.handle;
				send_server_resp.len = 0;
				APP_INFO1("send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
				APP_INFO1("send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
				APP_INFO1("send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
				BSA_BleSeSendRsp(&send_server_resp);
			}
			err = mozart_event_send(mozart_ble_se_write_event);
			if (err < 0)
				printf("Send ble write complete event failure\n");

			break;

		case BSA_BLE_SE_OPEN_EVT:
			printf("++++ BSA_BLE_SE_OPEN_EVT!\n");
			APP_INFO1("status:%d", p_data->ser_open.reason);
			if (p_data->ser_open.reason == BSA_SUCCESS)
			{
				APP_INFO1("conn_id:0x%x", p_data->ser_open.conn_id);
				num = app_ble_server_find_index_by_interface(p_data->ser_open.server_if);
				/* search interface number */
				if(num < 0)
				{
					APP_ERROR0("no interface!!");
					break;
				}
				app_ble_cb.ble_server[num].conn_id = p_data->ser_open.conn_id;
			}
			break;

		case BSA_BLE_SE_CLOSE_EVT:
			printf("++++ BSA_BLE_SE_CLOSE_EVT!\n");
			APP_INFO1("status:%d", p_data->ser_close.reason);
			APP_INFO1("conn_id:0x%x", p_data->ser_close.conn_id);
			num = app_ble_server_find_index_by_interface(p_data->ser_close.server_if);
			/* search interface number */
			if(num < 0)
			{
				APP_ERROR0("no interface!!");
				break;
			}
			app_ble_cb.ble_server[num].conn_id = BSA_BLE_INVALID_CONN;

		default:
			break;
	}
}

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
int app_ble_server_open(void)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_OPEN ble_open_param;
	int device_index;
	BD_ADDR bd_addr;
	int server_num;
	int direct;

	APP_INFO0("Bluetooth BLE connect menu:");
	APP_INFO0("    0 Device from XML database (already paired)");
	APP_INFO0("    1 Device found in last discovery");
	device_index = app_get_choice("Select source");
	/* Devices from XML database */
	if (device_index == 0)
	{
		/* Read the XML file which contains all the bonded devices */
		app_read_xml_remote_devices();

		app_xml_display_devices(app_xml_remote_devices_db,
				APP_NUM_ELEMENTS(app_xml_remote_devices_db));
		device_index = app_get_choice("Select device");
		if ((device_index >= 0) &&
				(device_index < APP_NUM_ELEMENTS(app_xml_remote_devices_db)) &&
				(app_xml_remote_devices_db[device_index].in_use != FALSE))
		{
			bdcpy(bd_addr, app_xml_remote_devices_db[device_index].bd_addr);
		}
		else
		{
			APP_ERROR1("Bad Device Index:%d\n", device_index);
			return -1;
		}

	}
	/* Devices from Discovery */
	else if (device_index == 1)
	{
		app_disc_display_devices();
		device_index = app_get_choice("Select device");
		if ((device_index >= 0) &&
				(device_index < APP_DISC_NB_DEVICES) &&
				(app_discovery_cb.devs[device_index].in_use != FALSE))
		{
			bdcpy(bd_addr, app_discovery_cb.devs[device_index].device.bd_addr);
		}
	}
	else
	{
		APP_ERROR0("Bad choice [XML(0) or Disc(1) only]");
		return -1;
	}

	APP_INFO0("Select Server:");
	app_ble_server_display();
	server_num = app_get_choice("Select");

	if((server_num < 0) ||
			(server_num >= BSA_BLE_SERVER_MAX) ||
			(app_ble_cb.ble_server[server_num].enabled == FALSE))
	{
		APP_ERROR1("Wrong server number! = %d", server_num);
		return -1;
	}

	if (app_ble_cb.ble_server[server_num].conn_id != BSA_BLE_INVALID_CONN)
	{
		APP_ERROR1("Connection already exist, conn_id = %d",
				app_ble_cb.ble_server[server_num].conn_id );
		return -1;
	}

	status = BSA_BleSeConnectInit(&ble_open_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeConnectInit failed status = %d", status);
		return -1;
	}

	ble_open_param.server_if = app_ble_cb.ble_server[server_num].server_if;
	direct = app_get_choice("Direct connection:1, Background connection:0");
	if(direct == 1)
	{
		ble_open_param.is_direct = TRUE;
	}
	else if(direct == 0)
	{
		ble_open_param.is_direct = FALSE;
	}
	else
	{
		APP_ERROR1("Wrong selection! = %d", direct);
		return -1;
	}
	bdcpy(ble_open_param.bd_addr, bd_addr);

	status = BSA_BleSeConnect(&ble_open_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeConnect failed status = %d", status);
		return -1;
	}

	return 0;
}

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
int app_ble_server_close(void)
{
	tBSA_STATUS status;
	tBSA_BLE_SE_CLOSE ble_close_param;
	int server_num;

	APP_INFO0("Select Server:");
	app_ble_server_display();
	server_num = app_get_choice("Select");

	if((server_num < 0) ||
			(server_num >= BSA_BLE_SERVER_MAX) ||
			(app_ble_cb.ble_server[server_num].enabled == FALSE))
	{
		APP_ERROR1("Wrong server number! = %d", server_num);
		return -1;
	}
	status = BSA_BleSeCloseInit(&ble_close_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeCloseInit failed status = %d", status);
		return -1;
	}
	ble_close_param.conn_id = app_ble_cb.ble_server[server_num].conn_id;
	status = BSA_BleSeClose(&ble_close_param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_BleSeClose failed status = %d", status);
		return -1;
	}

	return 0;
}

