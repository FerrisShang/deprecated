/*****************************************************************************
 **
 **  Name:           app_avk_main.c
 **
 **  Description:    Bluetooth Manager application
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "bsa_api.h"

#include "gki.h"
#include "uipc.h"

#include "app_avk.h"
#include "bsa_avk_api.h"
#include "app_xml_param.h"
#include "app_mgt.h"
#include "app_disc.h"
#include "app_utils.h"
#if 0
/*******************************************************************************
 **
 ** Function         app_avk_mgt_callback
 **
 ** Description      This callback function is called in case of server
 **                  disconnection (e.g. server crashes)
 **
 ** Parameters
 **
 ** Returns          TRUE if the event was completely handle, FALSE otherwise
 **
 *******************************************************************************/
static BOOLEAN app_avk_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
    switch(event)
    {
    case BSA_MGT_STATUS_EVT:
        APP_DEBUG0("BSA_MGT_STATUS_EVT");
        if (p_data->status.enable)
        {
            APP_DEBUG0("Bluetooth restarted => re-initialize the application");
            app_avk_init(NULL);
        }
        break;

    case BSA_MGT_DISCONNECT_EVT:
        APP_DEBUG1("BSA_MGT_DISCONNECT_EVT reason:%d", p_data->disconnect.reason);
        /* Connection with the Server lost => Just exit the application */
        exit(-1);
        break;

    default:
        break;
    }
    return FALSE;
}
#endif
void mozart_bluetooth_avk_start_play(void)
{
	app_avk_play_start();
}

void mozart_bluetooth_avk_stop_play(void)
{
	app_avk_play_stop();
}

void mozart_bluetooth_avk_play_pause(void)
{
	if (app_avk_get_bluetooth_play_state()) {
		printf(">>>>>avk pause>>>>>\n\n");
		app_avk_play_pause();
	} else {
		printf(">>>>>avk  play>>>>>\n\n");
		app_avk_play_start();
	}
}

void mozart_bluetooth_avk_next_music(void)
{
	app_avk_play_next_track();
}

void mozart_bluetooth_avk_prev_music(void)
{
	app_avk_play_previous_track();
}

int mozart_bluetooth_avk_start_service(void)
{
	if (app_avk_init(NULL) == -1) {
		APP_ERROR0("app_avk_init() error.\n");
		return -1;
	}

	/* Example to register AVK connection end point*/
	if (app_avk_register() == -1) {
		APP_ERROR0("app_disc_start_regular() error.\n");
		return -1;
	}

	return 0;
}

int mozart_bluetooth_avk_stop_service(void)
{
	/* Example to de-register an AVK end point */
	app_avk_deregister();

	/* Terminate the profile */
	app_avk_end();

	return 0;
}

int mozart_bluetooth_get_avk_play_state(void)
{
	return app_avk_get_bluetooth_play_state();
}
