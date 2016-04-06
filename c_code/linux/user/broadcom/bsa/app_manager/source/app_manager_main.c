/*****************************************************************************
 **
 **  Name:           app_manager_main.c
 **
 **  Description:    Bluetooth Manager menu application
 **
 **  Copyright (c) 2010-2014, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>

#include "gki.h"
#include "uipc.h"
#include "bsa_api.h"
#include "app_xml_utils.h"
#include "app_disc.h"
#include "app_utils.h"
#include "app_dm.h"
#include "app_services.h"
#include "app_mgt.h"
#include "app_hs.h"
#include "app_avk.h"
#include "app_manager.h"
#include "sharememory_interface.h"
/*
 * Extern variables
 */
extern BD_ADDR                 app_sec_db_addr;    /* BdAddr of peer device requesting SP */
extern tAPP_MGR_CB app_mgr_cb;
static tBSA_SEC_IO_CAP g_sp_caps = 0;
char *bt_dev_name = NULL;
extern tAPP_XML_CONFIG         app_xml_config;

enum {
	USE_HS_AVK = 0,
	USE_HS_ONLY,
	USE_AVK_ONLY
};

/*
 * Local functions
 */
/*******************************************************************************
 **
 ** Function         app_mgr_mgt_callback
 **
 ** Description      This callback function is called in case of server
 **                  disconnection (e.g. server crashes)
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
static BOOLEAN app_mgr_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
	switch(event)
	{
		case BSA_MGT_STATUS_EVT:
			printf("+++ BSA_MGT_STATUS_EVT !\n");
			if (p_data->status.enable)
			{
				APP_DEBUG0("Bluetooth restarted => re-initialize the application");
				app_mgr_config(bt_dev_name);
			}
			break;

		case BSA_MGT_DISCONNECT_EVT:
			printf("+++ BSA_MGT_DISCONNECT_EVT !\n");
			APP_DEBUG1("BSA_MGT_DISCONNECT_EVT reason:%d", p_data->disconnect.reason);
			exit(-1);
			break;

		default:
			break;
	}
	return FALSE;
}

int mozart_bluetooth_init(bt_init_info *bt_info)
{
	int mode = 0;

	/* Init App manager */
	app_mgt_init();
	if (app_mgt_open("/var/run/", app_mgr_mgt_callback) < 0) {
		APP_ERROR0("Unable to connect to server");
		return -1;
	}

	/* Init XML state machine */
	app_xml_init();

#ifndef BT_ALONE
	if (share_mem_init()) {
		printf("share_mem_init failure.\n");
		return -1;
	}
#endif
	bt_dev_name = bt_info->bt_name;
	if (app_mgr_xml_config(bt_info)) {
		APP_ERROR0("Couldn't configure, exiting");
		return -1;
	}
	g_sp_caps = app_xml_config.io_cap;

	/* Display FW versions */
	if (app_mgr_read_version()) {
		APP_ERROR0("Couldn't read versions, exiting");
		return -1;
	}

	/* Get the current Stack mode */
	if ((mode = app_dm_get_dual_stack_mode()) == -1) {
		APP_ERROR0("app_dm_get_dual_stack_mode failed");
		return -1;
	}

	/* Save the current DualStack mode */
	app_mgr_cb.dual_stack_mode = mode;
	APP_INFO1("Current DualStack mode:%s", app_mgr_get_dual_stack_mode_desc());

	return 0;
}

int mozart_bluetooth_uninit()
{
	/* Close BSA before exiting (to release resources) */
	app_mgt_close();

	app_mgt_kill_server();

	return 0;
}

/* Returns: 0 if success, -1 if failure */
int mozart_bluetooth_auto_reconnect(int type)
{
	open_device0_enable = 1;

	if ((type == USE_HS_ONLY) || (type == USE_HS_AVK))
		return app_hs_open_device0(NULL);
	else if (type == USE_AVK_ONLY)
		return app_avk_open_device0();
	else {
		printf("not support this type: %d\n", type);
		return -1;
	}
}

/* linked : TRUE, unlink : FALSE */
int mozart_bluetooth_get_link_status(void)
{
	return app_mgr_get_bt_link_status();
}

int mozart_bluetooth_disconnect(int type)
{
	if (mozart_bluetooth_get_link_status()) {
		if (app_avk_get_bluetooth_play_state() == TRUE)
			app_avk_play_stop();
		if (type == USE_HS_AVK) {
			app_avk_close();
			app_hs_close();
			return 0;
		} else if (type == USE_HS_ONLY) {
			app_hs_close();
			return 0;
		} else if (type == USE_AVK_ONLY) {
			app_avk_close();
			return 0;
		} else {
			printf("not support this type: %d\n", type);
			return -1;
		}
	}
	return 0;
}

