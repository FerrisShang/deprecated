/*****************************************************************************
**
**  Name:           app_hs_main.c
**
**  Description:    Bluetooth Manager application
**
**  Copyright (c) 2009-2012, Broadcom Corp., All Rights Reserved.
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
#include <pthread.h>

#include "bsa_api.h"
#include "app_hs.h"
#include "gki.h"
#include "uipc.h"
#include "app_utils.h"
#include "app_xml_param.h"
#include "app_xml_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_dm.h"

#define APP_HS_SCO_IN_SOUND_FILE    "/usr/data/bsa/sco_in.wav"
#define APP_HS_SCO_OUT_SOUND_FILE    APP_HS_SCO_IN_SOUND_FILE

extern int mozart_get_bluetooth_link_status(void);

int mozart_bluetooth_hs_get_call_state(void)
{
	return app_hs_get_bluetooth_call_state();
}

int mozart_bluetooth_hs_answer_call(void)
{
	return app_hs_answer_call();
}

int mozart_bluetooth_hs_hangup(void)
{
	return app_hs_hangup();
}

int mozart_blutooth_hs_set_volume(tBSA_BTHF_VOLUME_TYPE_T type, int volume)
{
	return app_hs_set_volume(type, volume);
}

int mozart_bluetooth_read_last_device_info(bd_last_info *bd_last_info)
{
	return app_xml_find_last_device_info(bd_last_info);
}

int mozart_bluetooth_hs_start_service(void)
{
    /* Example of function to get the Local Bluetooth configuration */
    if( app_dm_get_local_bt_config() < 0) {
	    APP_ERROR0("app_dm_get_local_bt_config failed");
	    return -1;
    }

    /* Init Headset Application */
    app_hs_init();

    /* Start Headset service*/
    if (app_hs_start(NULL)) {
	    APP_ERROR0("app_hs_start failed");
	    return -1;
    }

    return 0;
}

int mozart_bluetooth_hs_stop_service(void)
{
	app_hs_stop();

	return 0;
}

int mozart_bluetooth_hs_hold_call(tBSA_BTHF_CHLD_TYPE_T type)
{
	return app_hs_hold_call(type);
}

int mozart_bluetooth_hs_audio_open()
{
	return app_hs_audio_open();
}

int mozart_bluetooth_hs_audio_close()
{
	return app_hs_audio_close();
}
