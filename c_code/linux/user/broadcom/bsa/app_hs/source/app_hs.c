/*****************************************************************************
 **
 **  Name:           app_hs.c
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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "bsa_api.h"

#include "gki.h"
#include "uipc.h"

#include "app_utils.h"
#include "app_xml_param.h"
#include "app_xml_utils.h"

#include "app_disc.h"
#include "app_hs.h"
#include "app_dm.h"
#include "app_wav.h"
#include "btm_api.h"
#include "bta_api.h"
#include "bt_audio_hw.h"
#ifdef PCM_ALSA
#include "alsa/asoundlib.h"
#endif

#include "event_interface.h"
#include "ini_interface.h"
#include "volume_interface.h"
#include "sharememory_interface.h"
#include "app_manager.h"

enum {
	/* means not currently in call set up */
	CALLSETUP_STATE_NO_CALL_SETUP = 0,
	/* means an incoming call process ongoing */
	CALLSETUP_STATE_INCOMMING_CALL,
	/* means an outgoing call set up is ongoing */
	CALLSETUP_STATE_OUTGOING_CALL,
	/* means remote party being alerted in an outgoing call */
	CALLSETUP_STATE_REMOTE_BEING_ALERTED_IN_OUTGOING_CALL,
	/* means a call is waiting */
	CALLSETUP_STATE_WAITING_CALL,

	/* means there are no calls in progress */
	CALL_STATE_NO_CALLS_ONGOING,
	/* means at least one call is in progress */
	CALL_STATE_LEAST_ONE_CALL_ONGOING,

	/* No calls held */
	CALLHELD_STATE_NO_CALL_HELD,
	/* Call is placed on hold or active/held calls swapped */
	CALLHELD_STATE_PLACED_ON_HOLD_OR_SWAPPED,
	/* Call on hold, no active call */
	CALLHELD_STATE_CALL_ON_HOLD_AND_NO_ACTIVE_CALL,

} BSA_CALL_STATE;

extern void app_avk_play_stop(void);
extern int app_avk_get_bluetooth_play_state(void);
/* ui keypress definition */
enum
{
	APP_HS_KEY_OPEN = 1,
	APP_HS_KEY_CLOSE,
	APP_HS_KEY_PRESS,
	APP_HS_KEY_PLAY,
	APP_HS_KEY_RECORD,
	APP_HS_KEY_STOP_RECORDING,
	APP_HS_KEY_QUIT = 99
};

bd_last_info custom_last_info;
int bsa_connect_success = 0;

mozart_event mozart_hs_connected_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "connected",
		},
	},
	.type = EVENT_MISC,
};

mozart_event mozart_hs_disconnected_event = {
	.event = {
		.misc = {
			.name = "bluetooth",
			.type = "disconnected",
		},
	},
	.type = EVENT_MISC,
};

#define APP_HS_SAMPLE_RATE      8000      /* AG Voice sample rate is always 8KHz */
#define APP_HS_BITS_PER_SAMPLE  16        /* AG Voice sample size is 16 bits */
#define APP_HS_CHANNEL_NB       1         /* AG Voice sample in mono */
#define APP_HS_CALL_VOLUME_MAX	15
#define APP_HS_FEATURES  ( BSA_HS_FEAT_ECNR | BSA_HS_FEAT_3WAY | BSA_HS_FEAT_CLIP | \
		BSA_HS_FEAT_VREC | BSA_HS_FEAT_RVOL | BSA_HS_FEAT_ECS | \
		BSA_HS_FEAT_ECC | BSA_HS_FEAT_UNAT )

#define APP_HS_MIC_VOL  7
#define APP_HS_SPK_VOL  7

#define APP_HS_SCO_IN_SOUND_FILE    "usr/data/bsa/sco_in.wav"
#define APP_HS_SCO_OUT_SOUND_FILE    APP_HS_SCO_IN_SOUND_FILE

#define APP_HS_HSP_SERVICE_NAME "BSA Headset"
#define APP_HS_HFP_SERVICE_NAME "BSA Handsfree"

#define APP_HS_MAX_AUDIO_BUF 240

#define APP_HS_XML_REM_DEVICES_FILE_PATH       "/usr/data/bsa/bt_devices.xml"

/*
 * Types
 */

/* control block (not needed to be stored in NVRAM) */
typedef struct
{
	tBSA_HS_CONN_CB    	conn_cb[BSA_HS_MAX_NUM_CONN];
	BOOLEAN            	registered;
	BOOLEAN            	is_muted;
	BOOLEAN            	mute_inband_ring;
	UINT32             	ring_handle;
	UINT32             	cw_handle;
	UINT32             	call_op_handle;
	UINT8              	sco_route;
	int                	rec_fd; /* recording file descriptor */
	int                	data_size;
	short		audio_buf[APP_HS_MAX_AUDIO_BUF];
	BOOLEAN		open_pending;
	struct device_prope	DevPrope;
} tAPP_HS_CB;

/*
 * Globales Variables
 */
tAPP_HS_CB  app_hs_cb;
int bluetooth_call_state = CALL_STATE_NO_CALLS_ONGOING;

const char *app_hs_service_ind_name[] =
{
	"NO SERVICE",
	"SERVICE AVAILABLE"
};

const char *app_hs_call_ind_name[] =
{
	"NO CALL",
	"ACTIVE CALL"
};

const char *app_hs_callsetup_ind_name[] =
{
	"CALLSETUP DONE",
	"INCOMING CALL",
	"OUTGOING CALL",
	"ALERTING REMOTE"
};

const char *app_hs_callheld_ind_name[] =
{
	"NONE ON-HOLD",
	"ACTIVE+HOLD",
	"ALL ON-HOLD"
};

const char *app_hs_roam_ind_name[] =
{
	"HOME NETWORK",
	"ROAMING"
};

/* application callback */
static tHsCallback *s_pHsCallback = NULL;

#ifdef PCM_ALSA
static char *alsa_device = "default"; /* ALSA playback device */
static snd_pcm_t *alsa_handle_playback = NULL;
static snd_pcm_t *alsa_handle_capture = NULL;
static BOOLEAN alsa_capture_opened = FALSE;
static BOOLEAN alsa_playback_opened = FALSE;
#endif

#ifdef PCM_ALSA
int app_hs_open_alsa_duplex(void);
int app_hs_close_alsa_duplex(void);
#endif

int app_hs_get_bluetooth_call_state()
{
	return bluetooth_call_state;
}
/*******************************************************************************
 **
 ** Function         app_hs_get_default_conn
 **
 ** Description      Find the first active connection control block
 **
 ** Returns          Pointer to the found connection, NULL if not found
 *******************************************************************************/
tBSA_HS_CONN_CB *app_hs_get_default_conn()
{
	UINT16 cb_index;

	APPL_TRACE_EVENT0("app_hs_get_default_conn");

	for(cb_index = 0; cb_index < BSA_HS_MAX_NUM_CONN; cb_index++)
	{
		if(app_hs_cb.conn_cb[cb_index].connection_active)
			return &app_hs_cb.conn_cb[cb_index];
	}
	return NULL;
}

/*******************************************************************************
 **
 ** Function         app_hs_get_conn_by_handle
 **
 ** Description      Find a connection control block by its handle
 **
 ** Returns          Pointer to the found connection, NULL if not found
 *******************************************************************************/
static tBSA_HS_CONN_CB *app_hs_get_conn_by_handle(UINT16 handle)
{
	APPL_TRACE_EVENT1("app_hs_get_conn_by_handle: %d", handle);

	/* check that the handle does not go beyond limits */
	if (handle <= BSA_HS_MAX_NUM_CONN)
	{
		return &app_hs_cb.conn_cb[handle-1];
	}

	return NULL;
}

/*******************************************************************************
 **
 ** Function         app_hs_find_indicator_id
 **
 ** Description      parses the indicator string and finds the position of a field
 **
 ** Returns          index in the string
 *******************************************************************************/
static UINT8 app_hs_find_indicator_id(char * ind, char * field)
{
	UINT16 string_len = strlen(ind);
	UINT8 i, id = 0;
	BOOLEAN skip = FALSE;

	for(i=0; i< string_len ; i++)
	{
		if(ind[i] == '"')
		{
			if(!skip)
			{
				id++;
				if(!strncmp(&ind[i+1], field, strlen(field)) && (ind[i+1+strlen(field)] == '"'))
				{

					return id;
				}
				else
				{
					/* skip the next " */
					skip = TRUE;
				}
			}
			else
			{
				skip = FALSE;
			}
		}
	}
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_decode_indicator_string
 **
 ** Description      process the indicator string and sets the indicator ids
 **
 ** Returns          void
 *******************************************************************************/
static void app_hs_decode_indicator_string(tBSA_HS_CONN_CB *p_conn, char * ind)
{
	p_conn->call_ind_id = app_hs_find_indicator_id(ind, "call");
	p_conn->call_setup_ind_id = app_hs_find_indicator_id(ind, "callsetup");
	if(!p_conn->call_setup_ind_id)
	{
		p_conn->call_setup_ind_id = app_hs_find_indicator_id(ind, "call_setup");
	}
	p_conn->service_ind_id = app_hs_find_indicator_id(ind, "service");
	p_conn->battery_ind_id = app_hs_find_indicator_id(ind, "battchg");
	p_conn->callheld_ind_id = app_hs_find_indicator_id(ind, "callheld");
	p_conn->signal_strength_ind_id = app_hs_find_indicator_id(ind, "signal");
	p_conn->roam_ind_id = app_hs_find_indicator_id(ind, "roam");

	printf("p_conn->call_ind_id = %d, p_conn->call_setup_ind_id = %d, p_conn->service_ind_id = %d, p_conn->battery_ind_id = %d\n",
			p_conn->call_ind_id, p_conn->call_setup_ind_id, p_conn->service_ind_id, p_conn->battery_ind_id);
	printf("p_conn->callheld_ind_id = %d, p_conn->signal_strength_ind_id = %d, p_conn->roam_ind_id = %d\n",
			p_conn->callheld_ind_id, p_conn->signal_strength_ind_id, p_conn->roam_ind_id);
}

/*******************************************************************************
 **
 ** Function         app_hs_set_indicator_status
 **
 ** Description      sets the current indicator
 **
 ** Returns          void
 *******************************************************************************/
static void app_hs_set_initial_indicator_status(tBSA_HS_CONN_CB *p_conn, char * ind)
{
	UINT8 i, pos;

	/* Clear all indicators. Not all indicators will be initialized */
	p_conn->curr_call_ind = 0;
	p_conn->curr_call_setup_ind = 0;
	p_conn->curr_service_ind = 0;
	p_conn->curr_callheld_ind = 0;
	p_conn->curr_signal_strength_ind = 0;
	p_conn->curr_roam_ind = 0;
	p_conn->curr_battery_ind = 0;

	/* skip any spaces in the front */
	while ( *ind == ' ' ) ind++;

	/* get "call" indicator*/
	pos = p_conn->call_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_call_ind = ind[i] - '0';
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	/* get "callsetup" indicator*/
	pos = p_conn->call_setup_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_call_setup_ind = ind[i] - '0';
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	/* get "service" indicator*/
	pos = p_conn->service_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_service_ind = ind[i] - '0';
			/* if there is no service play the designated tone */
			if(!p_conn->curr_service_ind)
			{
				/*
				   if(HS_CFG_BEEP_NO_NETWORK)
				   {
				   UTL_BeepPlay(HS_CFG_BEEP_NO_NETWORK);
				   }*/
			}
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	/* get "callheld" indicator*/
	pos = p_conn->callheld_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_callheld_ind = ind[i] - '0';
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	/* get "signal" indicator*/
	pos = p_conn->signal_strength_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_signal_strength_ind = ind[i] - '0';
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	/* get "roam" indicator*/
	pos = p_conn->roam_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_roam_ind = ind[i] - '0';
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	/* get "battchg" indicator*/
	pos = p_conn->battery_ind_id -1;
	for(i=0; i< strlen(ind) ; i++)
	{
		if(!pos)
		{
			p_conn->curr_battery_ind = ind[i] - '0';
			break;
		}
		else if(ind[i] == ',')
			pos--;
	}

	if(p_conn->curr_callheld_ind != 0)
	{
		BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_3WAY_HELD);
	}
	else if(p_conn->curr_call_ind == BSA_HS_CALL_ACTIVE)
	{
		if(p_conn->curr_call_setup_ind == BSA_HS_CALLSETUP_INCOMING)
		{
			BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_WAITCALL);
		}
		else
		{
			BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_CALLACTIVE);
		}
	}
	else if(p_conn->curr_call_setup_ind == BSA_HS_CALLSETUP_INCOMING)
	{
		BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_RINGACT);
	}
	else if((p_conn->curr_call_setup_ind == BSA_HS_CALLSETUP_OUTGOING) ||
			(p_conn->curr_call_setup_ind == BSA_HS_CALLSETUP_ALERTING)
	       )
	{
		BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_OUTGOINGCALL);
	}

	/* Dump indicators */
	APPL_TRACE_EVENT2("Service: %s,%d", app_hs_service_ind_name[p_conn->curr_service_ind],p_conn->curr_service_ind);
	APPL_TRACE_EVENT2("Call: %s,%d", app_hs_call_ind_name[p_conn->curr_call_ind],p_conn->curr_call_ind);
	APPL_TRACE_EVENT2("Callsetup: Ind %s,%d", app_hs_callsetup_ind_name[p_conn->curr_call_setup_ind],p_conn->curr_call_setup_ind);
	APPL_TRACE_EVENT2("Hold: %s,%d", app_hs_callheld_ind_name[p_conn->curr_callheld_ind],p_conn->curr_callheld_ind);
	APPL_TRACE_EVENT2("Roam: %s,%d", app_hs_roam_ind_name[p_conn->curr_roam_ind],p_conn->curr_roam_ind);
}

/*******************************************************************************
 **
 ** Function         app_hs_write_to_file
 **
 ** Description      write SCO IN data to file
 **
 ** Parameters
 **
 ** Returns          number of byte written.
 **
 *******************************************************************************/
static int app_hs_write_to_file(UINT8 *p_buf, int size)
{
#ifdef PCM_ALSA
	APP_ERROR0("Cannot write to file when PCM_ALSA is defined");
	return -1;
#endif

	int ret;
	if(app_hs_cb.rec_fd <= 0)
	{
		APP_DEBUG0("no file to write...\n");
		return 0;
	}

	ret = write(app_hs_cb.rec_fd, p_buf, size);

	if(ret != size)
	{
		APP_ERROR1("write failed with code %d, fd %d", ret, app_hs_cb.rec_fd);
	}


	return ret;
}

/*******************************************************************************
 **
 ** Function         app_hs_sco_uipc_cback
 **
 ** Description     uipc audio call back function.
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
static void app_hs_sco_uipc_cback(BT_HDR *p_buf)
{
	UINT8 *pp = (UINT8 *)(p_buf + 1);
	UINT8 pkt_len;

#ifdef PCM_ALSA
	snd_pcm_sframes_t alsa_frames;
	snd_pcm_sframes_t alsa_frames_expected;
#endif

	if (p_buf == NULL)
	{
		return;
	}

	pkt_len = p_buf->len;

	if (app_hs_cb.rec_fd>0)
	{
		app_hs_write_to_file(pp, pkt_len);
	}
#ifdef PCM_ALSA
	/* Compute number of PCM samples (contained in pkt_len->len bytes) */
	/* Divide by the number of channel */
	alsa_frames_expected = pkt_len / APP_HS_CHANNEL_NB;
	alsa_frames_expected /= 2; /* 16 bits samples */

	if (alsa_playback_opened != FALSE)
	{
		/*
		 * Send PCM samples to ALSA/asound driver (local sound card)
		 */
		alsa_frames = snd_pcm_writei(alsa_handle_playback, pp, alsa_frames_expected);
		if (alsa_frames < 0)
		{
			APP_DEBUG1("snd_pcm_recover %d", (int)alsa_frames);
			alsa_frames = snd_pcm_recover(alsa_handle_playback, alsa_frames, 0);
		}
		if (alsa_frames < 0)
		{
			APP_ERROR1("snd_pcm_writei failed: %s", snd_strerror(alsa_frames));
		}
		if (alsa_frames > 0 && alsa_frames < alsa_frames_expected)
		{
			APP_ERROR1("Short write (expected %d, wrote %d)",
					(int)alsa_frames_expected, (int)alsa_frames);
		}
	}
	else
	{
		APP_DEBUG0("alsa_playback_opened NOT");
	}

	if (alsa_capture_opened != FALSE)
	{
		/*
		 * Read PCM samples from ALSA/asound driver (local sound card)
		 */
		alsa_frames = snd_pcm_readi(alsa_handle_capture, app_hs_cb.audio_buf, alsa_frames_expected);
		if (alsa_frames < 0)
		{
			APP_ERROR1("snd_pcm_readi returns: %d", (int)alsa_frames);
		}
		else if ((alsa_frames > 0) &&
				(alsa_frames < alsa_frames_expected))
		{
			APP_ERROR1("Short read (expected %i, wrote %i)", (int)alsa_frames_expected, (int)alsa_frames);
		}
		/* Send them to UIPC (to Headet) */

		/* for now we just handle one instance */
		/* for multiple instance user should be prompted */
		tBSA_HS_CONN_CB * p_conn = &(app_hs_cb.conn_cb[0]);

		if (TRUE != UIPC_Send(p_conn->uipc_channel, 0, (UINT8 *) app_hs_cb.audio_buf, alsa_frames * 2))
		{
			APP_ERROR0("UIPC_Send failed");
		}
	}
	else
	{
		APP_DEBUG0("alsa_capture NOT opened");
	}
#endif

	GKI_freebuf(p_buf);
}

/*******************************************************************************
 **
 ** Function         app_hs_read_xml_remote_devices
 **
 ** Description      This function is used to read the XML bluetooth remote device file
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_hs_read_xml_remote_devices(void)
{
	int status;
	int index;

	for (index = 0; index < APP_NUM_ELEMENTS(app_xml_remote_devices_db); index++)
	{
		app_xml_remote_devices_db[index].in_use = FALSE;
	}

	status = app_xml_read_db(APP_HS_XML_REM_DEVICES_FILE_PATH, app_xml_remote_devices_db,
			APP_NUM_ELEMENTS(app_xml_remote_devices_db));

	if (status < 0)
	{
		APP_ERROR1("app_xml_read_db failed (%d)", status);
		return -1;
	}
	return 0;
}

/* public function */

/*******************************************************************************
 **
 ** Function         app_hs_open_device0
 **
 ** Description      Establishes mono headset connections to device0 default.
 **
 ** Parameter        BD address to connect to. If its NULL, the app will prompt user for device.
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/
int app_hs_open_device0(BD_ADDR *bd_addr_in /*= NULL*/)
{
	tBSA_STATUS status = 0;
	BD_ADDR bd_addr;
	int device_index;
	tBSA_HS_OPEN param;

	if(bd_addr_in == NULL)
	{
		/* Devices from XML databased */
		device_index = 0;
		if (device_index == 0)
		{
			/* Read the Remote device xml file to have a fresh view */
			if (app_hs_read_xml_remote_devices() == -1)
				return -2;

			app_xml_display_devices(app_xml_remote_devices_db, APP_NUM_ELEMENTS(app_xml_remote_devices_db));
			if ((device_index >= 0) &&
					(device_index < APP_NUM_ELEMENTS(app_xml_remote_devices_db)) &&
					(app_xml_remote_devices_db[device_index].in_use != FALSE))
			{
				bdcpy(bd_addr, app_xml_remote_devices_db[device_index].bd_addr);
			}
			else
			{
				printf("Bad Device Index:%d\n", device_index);
				return -1;
			}
		}
		/* Devices from Discovery */
		else
		{
			app_disc_display_devices();
			printf("Enter device number\n");
			device_index = app_get_choice("Select device");
			if ((device_index >= 0) &&
					(device_index < APP_DISC_NB_DEVICES) &&
					(app_discovery_cb.devs[device_index].in_use != FALSE))
			{
				bdcpy(bd_addr, app_discovery_cb.devs[device_index].device.bd_addr);
			}
			else
			{
				printf("Bad Device Index:%d\n", device_index);
				return -1;
			}
		}
	}
	else
	{
		bdcpy(bd_addr, *bd_addr_in);
	}

	BSA_HsOpenInit(&param);

	bdcpy(param.bd_addr, bd_addr);
	/* we manage only one connection for now */
	param.hndl = app_hs_cb.conn_cb[0].handle;
	status = BSA_HsOpen(&param);
	app_hs_cb.open_pending = TRUE;

	if (status != BSA_SUCCESS)
	{
		printf("BSA_HsOpen failed (%d)", status);
		app_hs_cb.open_pending = FALSE;
	}
	return status;
}
/*******************************************************************************
 **
 ** Function         app_hs_open
 **
 ** Description      Establishes mono headset connections
 **
 ** Parameter        BD address to connect to. If its NULL, the app will prompt user for device.
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/
int app_hs_open(BD_ADDR *bd_addr_in /*= NULL*/)
{
	tBSA_STATUS status = 0;
	BD_ADDR bd_addr;
	int device_index;

	tBSA_HS_OPEN param;

	APP_DEBUG0("Entering");

	if(bd_addr_in == NULL)
	{
		printf("Bluetooth AG menu:\n");
		printf("    0 Device from XML database (already paired)\n");
		printf("    1 Device found in last discovery\n");

		device_index = app_get_choice("Select source");
		/* Devices from XML databased */
		if (device_index == 0)
		{
			/* Read the Remote device xml file to have a fresh view */
			app_hs_read_xml_remote_devices();

			app_xml_display_devices(app_xml_remote_devices_db, APP_NUM_ELEMENTS(app_xml_remote_devices_db));
			device_index = app_get_choice("Select device");
			if ((device_index >= 0) &&
					(device_index < APP_NUM_ELEMENTS(app_xml_remote_devices_db)) &&
					(app_xml_remote_devices_db[device_index].in_use != FALSE))
			{
				bdcpy(bd_addr, app_xml_remote_devices_db[device_index].bd_addr);
			}
			else
			{
				printf("Bad Device Index:%d\n", device_index);
				return -1;
			}
		}
		/* Devices from Discovery */
		else
		{
			app_disc_display_devices();
			printf("Enter device number\n");
			device_index = app_get_choice("Select device");
			if ((device_index >= 0) &&
					(device_index < APP_DISC_NB_DEVICES) &&
					(app_discovery_cb.devs[device_index].in_use != FALSE))
			{
				bdcpy(bd_addr, app_discovery_cb.devs[device_index].device.bd_addr);
			}
			else
			{
				printf("Bad Device Index:%d\n", device_index);
				return -1;
			}
		}
	}
	else
	{
		bdcpy(bd_addr, *bd_addr_in);
	}

	BSA_HsOpenInit(&param);

	bdcpy(param.bd_addr, bd_addr);
	/* we manage only one connection for now */
	param.hndl = app_hs_cb.conn_cb[0].handle;
	status = BSA_HsOpen(&param);
	app_hs_cb.open_pending = TRUE;

	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("BSA_HsOpen failed (%d)", status);
		app_hs_cb.open_pending = FALSE;
	}
	return status;
}

/*******************************************************************************
 **
 ** Function         app_hs_audio_open
 **
 ** Description      Open the SCO connection alone
 **
 ** Parameter        None
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/

int app_hs_audio_open(void)
{
	printf("app_hs_audio_open\n");

	tBSA_STATUS status;
	tBSA_HS_AUDIO_OPEN audio_open;
	tBSA_HS_CONN_CB * p_conn = &(app_hs_cb.conn_cb[0]);

	if(app_hs_cb.sco_route == BSA_SCO_ROUTE_HCI &&
			p_conn->uipc_channel == UIPC_CH_ID_BAD)
	{
		APP_ERROR0("Bad UIPC channel in app_hs_audio_open");
		return -1;
	}

	BSA_HsAudioOpenInit(&audio_open);
	audio_open.sco_route = app_hs_cb.sco_route;
	audio_open.hndl = app_hs_cb.conn_cb[0].handle;
	status = BSA_HsAudioOpen(&audio_open);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("failed with status : %d", status);
		return -1;
	}
	return status;
}

/*******************************************************************************
 **
 ** Function         app_hs_audio_close
 **
 ** Description      Close the SCO connection alone
 **
 ** Parameter        None
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/

int app_hs_audio_close(void)
{
	printf("app_hs_audio_close\n");
	tBSA_STATUS status;
	tBSA_HS_AUDIO_CLOSE audio_close;
	BSA_HsAudioCloseInit(&audio_close);
	audio_close.hndl = app_hs_cb.conn_cb[0].handle;
	status = BSA_HsAudioClose(&audio_close);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("failed with status : %d", status);
		return -1;
	}
	return status;
}

/*******************************************************************************
 **
 ** Function         app_hs_close
 **
 ** Description      release mono headset connections
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/
int app_hs_close(void)
{
	tBSA_HS_CLOSE param;
	tBSA_STATUS status;

	/* Prepare parameters */
	BSA_HsCloseInit(&param);

	/* todo : promt user to check what to close here */
	/* for now we just handle one AG at a time */
	param.hndl = app_hs_cb.conn_cb[0].handle;

	status = BSA_HsClose(&param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("failed with status : %d", status);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_cancel
 **
 ** Description      cancel connections
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/
int app_hs_cancel(void)
{
	tBSA_HS_CANCEL param;
	tBSA_STATUS status;

	/* Prepare parameters */
	BSA_HsCancelInit(&param);

	status = BSA_HsCancel(&param);
	if (status != BSA_SUCCESS)
	{
		APP_ERROR1("failed with status : %d", status);
		return -1;
	}

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_is_open_pending
 **
 **
 ** Returns          TRUE if open if pending
 *******************************************************************************/
BOOLEAN app_is_open_pending()
{
	return app_hs_cb.open_pending;
}

/*******************************************************************************
 **
 ** Function         app_hs_answer_call
 **
 ** Description      example of function to answer the call
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/
int app_hs_answer_call(void)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn;

	printf("app_hs_answer_call\n");

	/* If no connection exist, error */
	if ((p_conn = app_hs_get_default_conn()) == NULL)
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_A_CMD;
	BSA_HsCommand(&cmd_param);

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_hangup
 **
 ** Description      example of function to hang up
 **
 ** Returns          0 if success -1 if failure
 *******************************************************************************/
int app_hs_hangup(void)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn;

	printf("app_hs_hangup\n");

	bluetooth_call_state = CALL_STATE_NO_CALLS_ONGOING;
	/* If no connection exist, error */
	if ((p_conn = app_hs_get_default_conn()) == NULL)
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_CHUP_CMD;
	BSA_HsCommand(&cmd_param);

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_stop
 **
 ** Description      This function is used to stop hs services and close all
 **                  UIPC channel.
 **
 ** Parameters       void
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_hs_stop(void)
{
	tBSA_HS_DISABLE      disable_param;
	tBSA_HS_DEREGISTER   deregister_param;
	tBSA_HS_CONN_CB * p_conn;
	UINT8 index;

	BSA_HsDeregisterInit(&deregister_param);
	for(index=0; index<BSA_HS_MAX_NUM_CONN; index++)
	{
		p_conn = &(app_hs_cb.conn_cb[index]);
		deregister_param.hndl = p_conn->handle;
		APP_DEBUG1("handle %d", deregister_param.hndl);
		BSA_HsDeregister(&deregister_param);

		if(p_conn->uipc_channel != UIPC_CH_ID_BAD)
		{
			if(p_conn->uipc_connected)
			{
				APPL_TRACE_DEBUG0("Closing UIPC Channel");
				UIPC_Close(p_conn->uipc_channel);
				p_conn->uipc_connected = FALSE;
			}
			p_conn->uipc_channel = UIPC_CH_ID_BAD;
		}
	}

	BSA_HsDisableInit(&disable_param);
	BSA_HsDisable(&disable_param);

	/* for now we just handle one instance */
	/* for multiple instance user should be prompted */
}

/*******************************************************************************
 **
 ** Function         app_hs_close_rec_file
 **
 ** Description      Close recording file
 **
 ** Parameters       void
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_hs_close_rec_file(void)
{
	int fd;
	tAPP_WAV_FILE_FORMAT format;

	if(app_hs_cb.rec_fd <= 0)
		return;

	format.bits_per_sample = 8;
	format.bits_per_sample = APP_HS_BITS_PER_SAMPLE;
	format.nb_channels = APP_HS_CHANNEL_NB;
	format.sample_rate = APP_HS_SAMPLE_RATE;

	fd = app_hs_cb.rec_fd;
	app_hs_cb.rec_fd = 0;
	app_wav_close_file(fd, &format);
}

/*******************************************************************************
 **
 ** Function         app_hs_open_rec_file
 **
 ** Description     Open recording file
 **
 ** Parameters      filename to open
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_hs_open_rec_file(char * filename)
{
	app_hs_cb.rec_fd = app_wav_create_file(filename, 0);
}

/*******************************************************************************
 **
 ** Function         app_hs_play_file
 **
 ** Description      Play SCO data from file to SCO OUT channel
 **
 ** Parameters       char * filename
 **
 ** Returns          0 if success -1 if failure
 **
 *******************************************************************************/
int app_hs_play_file(char * filename)
{
	tAPP_WAV_FILE_FORMAT  wav_format;
	int nb_bytes = 0;
	int fd = 0;

	printf("app_hs_play_file\n");

	fd = app_wav_open_file(filename, &wav_format);

	if(fd < 0)
	{
		printf("Error could not open wav input file\n");
		printf("Use the Record audio file function to create an audio file called %s and then try again\n",APP_HS_SCO_OUT_SOUND_FILE);
		return -1;
	}

	do
	{
		nb_bytes = read(fd, app_hs_cb.audio_buf, sizeof(app_hs_cb.audio_buf)); /* read audio sample */

		if(nb_bytes < 0)
		{
			close(fd);
			return -1;
		}

		/* for now we just handle one instance */
		/* for multiple instance user should be prompted */
		tBSA_HS_CONN_CB * p_conn = &(app_hs_cb.conn_cb[0]);

		if (TRUE != UIPC_Send(p_conn->uipc_channel, 0,
					(UINT8 *) app_hs_cb.audio_buf,
					nb_bytes))
		{
			printf("error in UIPC send could not send data \n");
		}

	} while (nb_bytes != 0);

	close(fd);

	return 0;
}

/*******************************************************************************
 **
 ** Function        app_xml_find_last_device_info
 **
 ** Description     find the last connected devices
 **
 ** Returns         1 if successful, error code otherwise
 **
 *******************************************************************************/
int app_xml_find_last_device_info(bd_last_info *last_info)
{
	if (last_info) {
		strncpy((char *)last_info->bd_name, (char *)custom_last_info.bd_name, sizeof(last_info->bd_name) - 1);
		bdcpy(last_info->bd_addr, custom_last_info.bd_addr);
	}
	return 1;
}

/*******************************************************************************
 **
 ** Function         app_hs_cback
 **
 ** Description      Example of HS callback function
 **
 ** Parameters       event code and data
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_hs_cback(tBSA_HS_EVT event, tBSA_HS_MSG *p_data)
{
	int err;
	int volume = 0;
	int index;
	char vol_buf[8] = {};
	char buf[6] = {};
	UINT16 handle = 0;
	tBSA_HS_CONN_CB *p_conn;

	if (!p_data)
	{
		printf("app_hs_cback p_data = NULL for event:%d\n", event);
		return;
	}

	/* retrieve the handle of the connection for which this event */
	handle = p_data->hdr.handle;
	APPL_TRACE_DEBUG2("app_hs_cback event:%d for handle: %d", event, handle);

	/* retrieve the connection for this handle */
	p_conn = app_hs_get_conn_by_handle(handle);

	if (!p_conn)
	{
		printf("app_hs_cback: handle %d not supported\n", handle);
		return;
	}

	switch (event)
	{
		case BSA_HS_CONN_EVT:       /* Service level connection */
			fprintf(stdout, "+++ BSA_HS_CONN_EVT\n");
			app_hs_cb.open_pending = FALSE;
			printf("    - Remote bdaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_data->conn.bd_addr[0], p_data->conn.bd_addr[1],
					p_data->conn.bd_addr[2], p_data->conn.bd_addr[3],
					p_data->conn.bd_addr[4], p_data->conn.bd_addr[5]);
			printf("    - Service: \n");
			switch (p_data->conn.service)
			{
				case BSA_HSP_HS_SERVICE_ID:
					fprintf(stdout, "+++ BSA_HSP_HS_SERVICE_ID\n");
					break;
				case BSA_HFP_HS_SERVICE_ID:
					fprintf(stdout, "+++ BSA_HFP_HS_SERVICE_ID\n");
					break;
				default:
					printf("Not supported 0x%08x\n", p_data->conn.service);
					return;
					break;
			}

			/* check if this conneciton is already opened */
			if (p_conn->connection_active)
			{
				printf("BSA_HS_CONN_EVT: connection already opened for handle %d\n", handle);
				break;
			}
			bdcpy(p_conn->connected_bd_addr, p_data->conn.bd_addr);
			p_conn->handle = p_data->conn.handle;
			p_conn->connection_active = TRUE;
			p_conn->connected_hs_service_id = p_data->conn.service;
			p_conn->peer_feature = p_data->conn.peer_features;
			p_conn->status = BSA_HS_ST_CONNECT;

			tAPP_XML_REM_DEVICE *p_stored_device_db = app_xml_remote_devices_db;
			/* First look in Database if this device already exist */
			for (index = 0; index < APP_NUM_ELEMENTS(app_xml_remote_devices_db); index++)
			{
				if ((p_stored_device_db[index].in_use != FALSE) && (bdcmp(p_stored_device_db[index].bd_addr, p_data->conn.bd_addr) == 0)) {
					strncpy((char *)custom_last_info.bd_name, (char *)p_stored_device_db[index].name, sizeof(custom_last_info.bd_name) - 1);
					custom_last_info.bd_name[sizeof(custom_last_info.bd_name) - 1] = '\0';
					bdcpy(custom_last_info.bd_addr, p_stored_device_db[index].bd_addr);
					printf("custom_last_info.bd_name = %s\n", custom_last_info.bd_name);
					printf("custom_last_info.bd_addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
							custom_last_info.bd_addr[0], custom_last_info.bd_addr[1],
							custom_last_info.bd_addr[2], custom_last_info.bd_addr[3],
							custom_last_info.bd_addr[4], custom_last_info.bd_addr[5]);
					break;
				}
			}
			break;

		case BSA_HS_CLOSE_EVT:      /* Connection Closed (for info)*/
			fprintf(stdout, "+++ BSA_HS_CLOSE_EVT\n");
			/* Close event, reason BSA_HS_CLOSE_CLOSED or BSA_HS_CLOSE_CONN_LOSS */
			APP_DEBUG1("BSA_HS_CLOSE_EVT, reason %d", p_data->hdr.status);
			app_hs_cb.open_pending = FALSE;

			if (!p_conn->connection_active)
			{
				printf("BSA_HS_CLOSE_EVT: connection not opened for handle %d\n", handle);
				break;
			}
			p_conn->connection_active = FALSE;
			p_conn->indicator_string_received = FALSE;

			BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_CONNECTABLE);
#ifndef BT_ALONE
			if(share_mem_set(BT_HS_DOMAIN, STATUS_RUNNING))
				printf("share_mem_set failure.\n");
			if (bsa_connect_success == 1) {
				bsa_connect_success = 0;
				int discoverable = 1;
				int connectable = 1;
				app_dm_set_visibility(discoverable, connectable);
				/* Connect Event notify to event manager */
				err = mozart_event_send(mozart_hs_disconnected_event);
				if (err < 0)
					printf("Send hs disconnected event failure\n");
			}
#endif
			break;

		case BSA_HS_AUDIO_OPEN_EVT:     /* Audio Open Event */
			fprintf(stdout, "+++ BSA_HS_AUDIO_OPEN_EVT\n");
			if (app_mgr_get_bt_link_status()) {
				if (app_avk_get_bluetooth_play_state() == TRUE)
					app_avk_play_stop();
			}
			if(app_hs_cb.sco_route == BSA_SCO_ROUTE_HCI &&
					p_conn->uipc_channel != UIPC_CH_ID_BAD &&
					!p_conn->uipc_connected)
			{
				/* Open UIPC channel for TX channel ID */
				if(UIPC_Open(p_conn->uipc_channel, app_hs_sco_uipc_cback)!= TRUE)
				{
					APP_ERROR1("app_hs_register failed to open UIPC channel(%d)",
							p_conn->uipc_channel);
					break;
				}
				p_conn->uipc_connected = TRUE;
				UIPC_Ioctl(p_conn->uipc_channel,UIPC_REG_CBACK,app_hs_sco_uipc_cback);
			}
#ifdef PCM_ALSA
			if(BSA_HS_GETSTATUS(p_conn, BSA_HS_ST_SCOOPEN))
				app_hs_close_alsa_duplex();
			app_hs_open_alsa_duplex();
#endif
#ifndef BT_ALONE
			module_status status;
			if (share_mem_get(BT_HS_DOMAIN, &status)) {
				printf("share_mem_get BT_HS_DOMAIN failed!\n");
				break;
			} else {
				if (status != STATUS_BT_HS_AUD_REQ &&
						status != STATUS_BT_HS_AUD_RDY &&
						status != STATUS_BT_HS_AUD_INUSE) {
					if(share_mem_stop_other(BT_HS_DOMAIN)) {
						printf("share_mem_stop_other failure!!!!\n");
						break;
					}
					if(share_mem_set(BT_HS_DOMAIN, STATUS_BT_HS_AUD_REQ)) {
						printf("share_mem_set STATUS_BT_HS_AUD_REQ failed!\n");
						break;
					}
				} else {
				}
			}
			int try = 0;
			while(try++ < 500) {
				if (share_mem_get(BT_HS_DOMAIN, &status)) {
					printf("share_mem_get BT_HS_DOMAIN failed!\n");
					break;
				} else {
					if (status == STATUS_BT_HS_AUD_RDY || status == STATUS_BT_HS_AUD_INUSE) {
						break;
					}
					usleep(10 * 1000);
				}
			}
			if (try >= 500) {
				break;
			}
#endif
			if (mozart_ini_getkey("/usr/data/system.ini", "volume", "bt", vol_buf)) {
				printf("failed to parse /usr/data/system.ini, set BT volume to 10.\n");
				volume = 10;
			} else {
				volume = atoi(vol_buf);
			}
			mozart_volume_set(volume, BT_VOLUME);

			if (bt_start_call(&app_hs_cb.DevPrope)) {
				printf("Error: bt_start_call failed!\n");
				break;
			}
			if (share_mem_set(BT_HS_DOMAIN, STATUS_BT_HS_AUD_INUSE)) {
				printf("share_mem_set failure.\n");
				break;
			}
			p_conn->call_state = BSA_HS_CALL_CONN;
			BSA_HS_SETSTATUS(p_conn, BSA_HS_ST_SCOOPEN);
			break;

		case BSA_HS_AUDIO_CLOSE_EVT:         /* Audio Close event */
			fprintf(stdout, "+++ BSA_HS_AUDIO_CLOSE_EVT\n");
#ifdef PCM_ALSA
			app_hs_close_alsa_duplex();
#endif
			if (bt_end_call(&app_hs_cb.DevPrope)) {
				printf("Error: bt_end_call failed !\n");
				break;
			}

			if (share_mem_set(BT_HS_DOMAIN, STATUS_RUNNING)) {
				printf("share_mem_set failed!.\n");
				break;
			}
			if (!p_conn->connection_active)
			{
				printf("BSA_HS_AUDIO_CLOSE_EVT: connection not opened for handle %d\n", handle);
				break;
			}
			p_conn->call_state = BSA_HS_CALL_NONE;
			BSA_HS_RESETSTATUS(p_conn, BSA_HS_ST_SCOOPEN);
			if(p_conn->uipc_channel != UIPC_CH_ID_BAD &&
					p_conn->uipc_connected)
			{
				APPL_TRACE_DEBUG0("Closing UIPC Channel");
				UIPC_Close(p_conn->uipc_channel);
				p_conn->uipc_connected = FALSE;
			}
			break;

		case BSA_HS_CIEV_EVT:
			/* CIEV event */
			fprintf(stdout, "+++ BSA_HS_CIEV_EVT\n");
#ifndef BT_ALONE
			strncpy(buf, p_data->val.str, 4);
			buf[0] = '\0';
			buf[2] = '\0';
			printf("buf[1] = %d\n", atoi(buf + 1));  /* ind */
			printf("buf[3] = %d\n", atoi(buf + 3));  /* value */

			/* callsetup <value>=0, means No call setup in progress. */
			if ((atoi(buf + 1) == p_conn->call_setup_ind_id) && (buf[3] == '0')) {
				/* hang up when call is ring status */
				if ((bluetooth_call_state != CALL_STATE_LEAST_ONE_CALL_ONGOING)) {
					bluetooth_call_state = CALLSETUP_STATE_NO_CALL_SETUP;
					printf("CALLSETUP_STATE_NO_CALL_SETUP, app_hs_callsetup_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callsetup_ind_name[atoi(buf + 3)]);
				}
			/* callsetup: <value>=1 , means an incoming call process ongoing */
			} else if ((atoi(buf + 1) == p_conn->call_setup_ind_id) && (buf[3] == '1')) {
				/* answer call */
				if (bluetooth_call_state != CALLSETUP_STATE_WAITING_CALL) {
					bluetooth_call_state = CALLSETUP_STATE_INCOMMING_CALL;
					printf("CALLSETUP_STATE_INCOMMING_CALL, app_hs_callsetup_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callsetup_ind_name[atoi(buf + 3)]);
				}
			/* callsetup: <value>=2, means an outgoing call set up is ongoing */
			} else if ((atoi(buf + 1) == p_conn->call_setup_ind_id) && (buf[3] == '2')) {
				bluetooth_call_state = CALLSETUP_STATE_OUTGOING_CALL;
				printf("CALLSETUP_STATE_OUTGOING_CALL, app_hs_callsetup_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callsetup_ind_name[atoi(buf + 3)]);
			} else if ((atoi(buf + 1) == p_conn->call_setup_ind_id) && (buf[3] == '3')) {
				bluetooth_call_state = CALLSETUP_STATE_REMOTE_BEING_ALERTED_IN_OUTGOING_CALL;
				printf("CALLSETUP_STATE_REMOTE_BEING_ALERTED_IN_OUTGOING_CALL, app_hs_callsetup_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callsetup_ind_name[atoi(buf + 3)]);

			/* call: <value>=0, means call process has been hang up */
			} else if ((atoi(buf + 1) == p_conn->call_ind_id) && buf[3] == '0') {
				/* hang up call */
				bluetooth_call_state = CALL_STATE_NO_CALLS_ONGOING;
				printf("CALL_STATE_NO_CALLS_ONGOING, app_hs_call_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_call_ind_name[atoi(buf + 3)]);
			/* call: <value>=1 , means at least one call is in progress */
			} else if ((atoi(buf + 1) == p_conn->call_ind_id) && (buf[3] == '1')) {
				bluetooth_call_state = CALL_STATE_LEAST_ONE_CALL_ONGOING;
				printf("CALL_STATE_LEAST_ONE_CALL_ONGOING, app_hs_call_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_call_ind_name[atoi(buf + 3)]);
				if(share_mem_set(BT_HS_DOMAIN, STATUS_BT_HS_AUD_INUSE))
					printf("share_mem_set failure.\n");

			/* callheld: <value>=0, means No calls held */
			} else if ((atoi(buf + 1) == p_conn->callheld_ind_id) && (buf[3] == '0')) {
				bluetooth_call_state = CALLHELD_STATE_NO_CALL_HELD;
				printf("CALLHELD_STATE_NO_CALL_HELD, app_hs_callheld_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callheld_ind_name[atoi(buf + 3)]);
			/* callheld: <value>=1, means Call is placed on hold or active/held calls swapped */
			} else if ((atoi(buf + 1) == p_conn->callheld_ind_id) && (buf[3] == '1')) {
				bluetooth_call_state = CALLHELD_STATE_PLACED_ON_HOLD_OR_SWAPPED;
				printf("CALLHELD_STATE_PLACED_ON_HOLD_OR_SWAPPED, app_hs_callheld_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callheld_ind_name[atoi(buf + 3)]);
			/* callheld: <value>=2, means Call on hold, no active call */
			} else if ((atoi(buf + 1) == p_conn->callheld_ind_id) && (buf[3] == '2')) {
				bluetooth_call_state = CALLHELD_STATE_CALL_ON_HOLD_AND_NO_ACTIVE_CALL;
				printf("CALLHELD_STATE_CALL_ON_HOLD_AND_NO_ACTIVE_CALL, app_hs_callheld_ind_name[%d] = %s\n", atoi(buf + 3), app_hs_callheld_ind_name[atoi(buf + 3)]);
			}
#endif
			break;

		case BSA_HS_CIND_EVT:                /* CIND event */
			fprintf(stdout, "+++ BSA_HS_CIND_EVT\n");
			printf("Call Indicator %s\n", p_data->val.str);

			/* check if indicator configuration was received */
			if(p_conn->indicator_string_received)
			{
				app_hs_set_initial_indicator_status(p_conn, p_data->val.str);
			}
			else
			{
				p_conn->indicator_string_received = TRUE;
				app_hs_decode_indicator_string(p_conn, p_data->val.str);
			}
#ifndef BT_ALONE
			if(bsa_connect_success == 0) {
				bsa_connect_success = 1;
				int discoverable = 1;
				int connectable = 0;
				app_dm_set_visibility(discoverable, connectable);
				/* Connect Event notify to event manager */
				err = mozart_event_send(mozart_hs_connected_event);
				if (err < 0) {
					printf("Send hs connected event failure\n");
					break;
				}
			}
#endif
			break;

		case BSA_HS_RING_EVT:
			fprintf(stdout, "+++ BSA_HS_RING_EVT\n");
			break;

		case BSA_HS_CLIP_EVT:
			fprintf(stdout, "+++ BSA_HS_CLIP_EVT\n");
			break;

		case BSA_HS_BSIR_EVT:
			fprintf(stdout, "+++ BSA_HS_BSIR_EVT\n");
			break;

		case BSA_HS_BVRA_EVT:
			fprintf(stdout, "+++ BSA_HS_BVRA_EVT\n");
			break;

		case BSA_HS_CCWA_EVT:
			bluetooth_call_state = CALLSETUP_STATE_WAITING_CALL;
			printf("CALLSETUP_STATE_WAITING_CALL !\n");
			fprintf(stdout, "+++ Call waiting : BSA_HS_CCWA_EVT:%s\n", p_data->val.str);
			break;

		case BSA_HS_CHLD_EVT:
			fprintf(stdout, "+++ BSA_HS_CHLD_EVT\n");
			break;

		case BSA_HS_VGM_EVT:
			fprintf(stdout, "+++ BSA_HS_VGM_EVT\n");
			break;

		case BSA_HS_VGS_EVT:
			fprintf(stdout, "+++ BSA_HS_VGS_EVT\n");
			mozart_volume_set((p_data->val.num) * 100 / APP_HS_CALL_VOLUME_MAX, BT_VOLUME);
			break;

		case BSA_HS_BINP_EVT:
			fprintf(stdout, "+++ BSA_HS_BINP_EVT\n");
			break;

		case BSA_HS_BTRH_EVT:
			fprintf(stdout, "+++ BSA_HS_BTRH_EVT\n");
			break;

		case BSA_HS_CNUM_EVT:
			fprintf(stdout, "+++ BSA_HS_CNUM_EVT:%s\n",p_data->val.str);
			break;

		case BSA_HS_COPS_EVT:
			fprintf(stdout, "+++ BSA_HS_COPS_EVT:%s\n",p_data->val.str);
			break;

		case BSA_HS_CMEE_EVT:
			fprintf(stdout, "+++ BSA_HS_CMEE_EVT:%s\n", p_data->val.str);
			break;

		case BSA_HS_CLCC_EVT:
			fprintf(stdout, "+++ BSA_HS_CLCC_EVT:%s\n", p_data->val.str);
			break;

		case BSA_HS_UNAT_EVT:
			fprintf(stdout, "+++ BSA_HS_UNAT_EVT:%s\n",p_data->val.str);
			break;

		case BSA_HS_OK_EVT:
			fprintf(stdout, "+++ BSA_HS_OK_EVT: command value %d, %s\n",p_data->val.num, p_data->val.str);
			break;

		case BSA_HS_ERROR_EVT:
			fprintf(stdout, "+++ BSA_HS_ERROR_EVT\n");
			break;

		case BSA_HS_BCS_EVT:
			fprintf(stdout, "+++ BSA_HS_BCS_EVT: codec %d (%s)\n",p_data->val.num,
					(p_data->val.num == BSA_SCO_CODEC_MSBC) ? "mSBC":"CVSD");
			break;

		case BSA_HS_OPEN_EVT:
			fprintf(stdout, "+++ BSA_HS_OPEN_EVT\n");
			app_hs_cb.open_pending = FALSE;
			break;

		default:
			printf("app_hs_cback unknown event:%d\n", event);
			break;
	}
	fflush(stdout);

	/* forward callback to the registered application */
	if(s_pHsCallback)
		s_pHsCallback(event, p_data);
}

/*******************************************************************************
 **
 ** Function         app_hs_start
 **
 ** Description      Example of function to start the Headset application
 **
 ** Parameters		Callback for event notification (can be NULL, if NULL default will be used)
 **
 ** Returns          0 if ok -1 in case of error
 **
 *******************************************************************************/
int app_hs_start(tHsCallback cb)
{
	tBSA_STATUS status;

	status = app_hs_enable();
	if (status != 0)
	{
		return status;
	}

	status = app_hs_register();

	s_pHsCallback = cb;

	return status;
}

/*******************************************************************************
 **
 ** Function         app_hs_enable
 **
 ** Description      Example of function to start enable Hs service
 **
 ** Parameters       void
 **
 ** Returns          0 if ok -1 in case of error
 **
 *******************************************************************************/
int app_hs_enable(void)
{
	int                status = 0;
	tBSA_HS_ENABLE     enable_param;
	/* prepare parameters */
	BSA_HsEnableInit(&enable_param);
	enable_param.p_cback = app_hs_cback;

	status = BSA_HsEnable(&enable_param);
	if (status != BSA_SUCCESS)
	{
		APP_DEBUG1("BSA_HsEnable failes with status %d", status);
		return -1;
	}
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_register
 **
 ** Description      Example of function to start register one Hs instance
 **
 ** Parameters       void
 **
 ** Returns          0 if ok -1 in case of error
 **
 *******************************************************************************/
int app_hs_register(void)
{
	int index, status = 0;

	tBSA_HS_REGISTER   param;
	tBSA_HS_CONN_CB * p_conn;
	app_hs_cb.DevPrope.dsp_play_r_fd = -1;
	app_hs_cb.DevPrope.dsp_play_w_fd = -1;
	app_hs_cb.DevPrope.dsp1_r_fd = -1;
	app_hs_cb.DevPrope.dsp1_w_fd = -1;
	app_hs_cb.DevPrope.dsp_record_r_fd = -1;
	app_hs_cb.DevPrope.write_bt_call = FALSE;
	app_hs_cb.DevPrope.write_codec_call = FALSE;
	app_hs_cb.DevPrope.read_bt_call = FALSE;
	app_hs_cb.DevPrope.read_dmic_call = FALSE;

	APP_DEBUG0("start Register");

	//    app_hs_cb.sco_route = BSA_SCO_ROUTE_HCI;
	app_hs_cb.sco_route = BSA_SCO_ROUTE_PCM;

	/* prepare parameters */
	BSA_HsRegisterInit(&param);
	param.services = BSA_HSP_HS_SERVICE_MASK | BSA_HFP_HS_SERVICE_MASK;
	param.sec_mask = BSA_SEC_NONE;
	/* Use BSA_HS_FEAT_CODEC (for WBS) for SCO over PCM only, not for SCO over HCI*/
	param.features = APP_HS_FEATURES;
	if(app_hs_cb.sco_route == BSA_SCO_ROUTE_HCI)
		param.features &= ~BSA_HS_FEAT_CODEC;
	param.settings.ecnr_enabled = (param.features & BSA_HS_FEAT_ECNR) ? TRUE : FALSE;
	param.settings.mic_vol = APP_HS_MIC_VOL;
	param.settings.spk_vol = APP_HS_SPK_VOL;
	strncpy(param.service_name[0], APP_HS_HSP_SERVICE_NAME, BSA_HS_SERVICE_NAME_LEN_MAX);
	strncpy(param.service_name[1], APP_HS_HFP_SERVICE_NAME, BSA_HS_SERVICE_NAME_LEN_MAX);

	/* SCO routing options:  BSA_SCO_ROUTE_HCI or BSA_SCO_ROUTE_PCM */
	param.sco_route = app_hs_cb.sco_route;

	for(index=0; index<BSA_HS_MAX_NUM_CONN; index++)
	{
		status = BSA_HsRegister(&param);
		if (status != BSA_SUCCESS)
		{
			APP_ERROR1("Unable to register HS with status %d", status);
			return -1;
		}

		if (status != BSA_SUCCESS)
		{
			APP_ERROR1("BSA_HsRegister failed(%d)", status);
			return -1;
		}

		p_conn = &(app_hs_cb.conn_cb[index]);
		p_conn->handle = param.hndl;
		p_conn->uipc_channel = param.uipc_channel;

		APP_DEBUG1("Register complete handle %d, status %d, uipc [%d->%d], sco_route %d",
				param.hndl, status, p_conn->uipc_channel, param.uipc_channel, param.sco_route);
	}

	APP_DEBUG0("Register complete");

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_init
 **
 ** Description      Init Headset application
 **
 ** Parameters
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
void app_hs_init(void)
{
	UINT8 index;

	memset(&app_hs_cb, 0, sizeof(app_hs_cb));

	for(index=0; index<BSA_HS_MAX_NUM_CONN ; index++)
	{
		app_hs_cb.conn_cb[index].uipc_channel = UIPC_CH_ID_BAD;
	}
}

/*******************************************************************************
 **
 ** Function         app_hs_hold_call
 **
 ** Description      Hold active call
 **
 ** Parameters       void
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_hold_call(tBSA_BTHF_CHLD_TYPE_T type)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn;

	printf("app_hs_hold_call\n");

	/* If no connection exist, error */
	if ((p_conn = app_hs_get_default_conn()) == NULL)
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_CHLD_CMD;
	cmd_param.data.num = type;
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_last_num_dial
 **
 ** Description      Re-dial last dialed number
 **
 ** Parameters       void
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_last_num_dial(void)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn;

	printf("app_hs_answer_call\n");

	/* If no connection exist, error */
	if ((p_conn = app_hs_get_default_conn()) == NULL)
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_BLDN_CMD;

	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_dial_num
 **
 ** Description      Dial a phone number
 **
 ** Parameters       Phone number string
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_dial_num(const char *num)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn;

	printf("app_hs_answer_call\n");

	if((num == NULL) || (strlen(num) == 0))
	{
		APP_DEBUG0("empty number string");
		return -1;
	}

	/* If no connection exist, error */
	if ((p_conn = app_hs_get_default_conn()) == NULL)
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_D_CMD;

	strcpy(cmd_param.data.str, num);
	strcat(cmd_param.data.str, ";");
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_send_unat
 **
 ** Description      Send an unknown AT Command
 **
 ** Parameters       char *cCmd
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_unat(char *cCmd)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	printf("app_hs_send_unat:Command : %s\n", cCmd);

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())) || NULL==cCmd)
	{
		APP_DEBUG0("no connection available or invalid AT Command");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_UNAT_CMD;

	strncpy(cmd_param.data.str, cCmd, sizeof(cmd_param.data.str)-1);

	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_send_clcc
 **
 ** Description      Send CLCC AT Command for obtaining list of current calls
 **
 ** Parameters       None
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_clcc_cmd()
{
	printf("app_hs_send_clcc_cmd\n");
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	/* If no connection exist, error */
	if (NULL==(p_conn = app_hs_get_default_conn()))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_CLCC_CMD;
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_send_cops_cmd
 **
 ** Description      Send COPS AT Command to obtain network details and set network details
 **
 ** Parameters       char *cCmd
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_cops_cmd(char *cCmd)
{
	printf("app_hs_send_cops_cmd\n");
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	/* If no connection exist, error */
	if (NULL==(p_conn = app_hs_get_default_conn()))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	strncpy(cmd_param.data.str, cCmd, sizeof(cmd_param.data.str)-1);
	cmd_param.command = BSA_HS_COPS_CMD;
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_send_ind_cmd
 **
 ** Description      Send indicator (CIND) AT Command
 **
 ** Parameters       None
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_ind_cmd()
{
	printf("app_hs_send_cind_cmd\n");
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	/* If no connection exist, error */
	if (NULL==(p_conn = app_hs_get_default_conn()))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_CIND_CMD;
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_mute_unmute_microphone
 **
 ** Description      Send Mute / unmute commands
 **
 ** Returns          Returns SUCCESS or FAILURE on sending mute/unmute command
 *******************************************************************************/
int app_hs_mute_unmute_microphone(BOOLEAN bMute)
{
	char str[20];
	memset(str,'\0',sizeof(str));
	strcpy(str, "+CMUT=");

	if(bMute)
		strcat(str, "1");
	else
		strcat(str, "0");
	return app_hs_send_unat(str);
}

/*******************************************************************************
 **
 ** Function         app_hs_send_dtmf
 **
 ** Description      Send DTMF AT Command
 **
 ** Parameters       char dtmf (0-9, #, A-D)
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_dtmf(char dtmf)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	printf("app_hs_send_dtmf:Command : %x\n", dtmf);

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())) || '\0'==dtmf)
	{
		APP_DEBUG0("no connection available or invalid DTMF Command");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_VTS_CMD;

	cmd_param.data.str[0] = dtmf;
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_send_cnum
 **
 ** Description      Send CNUM AT Command
 **
 ** Parameters       No parameter needed
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_cnum(void)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	printf("app_hs_send_cnum:Command \n");

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())))
	{
		APP_DEBUG0("no connection available or invalid DTMF Command");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_CNUM_CMD;
	cmd_param.data.num=0;

	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_send_keypress_evt
 **
 ** Description      Send Keypress event
 **
 ** Parameters       char *cCmd - Keyboard sequence (0-9, * , #)
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_send_keypress_evt(char *cCmd)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	printf("app_hs_send_keypress_evt:Command \n");

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;
	cmd_param.command = BSA_HS_CKPD_CMD;
	strncpy(cmd_param.data.str, cCmd, sizeof(cmd_param.data.str)-1);

	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_start_voice_recognition
 **
 ** Description      Start voice recognition
 **
 ** Parameters       None
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_start_voice_recognition(void)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	printf("app_hs_start_voice_recognition:Command \n");

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	if(app_hs_cb.conn_cb[0].peer_feature & BSA_HS_PEER_FEAT_VREC)
	{
		BSA_HsCommandInit(&cmd_param);
		cmd_param.hndl = p_conn->handle;
		cmd_param.command = BSA_HS_BVRA_CMD;
		cmd_param.data.num = 1;

		BSA_HsCommand(&cmd_param);
		return 0;
	}

	APP_DEBUG0("Peer feature - VR feature not available");
	return -1;
}


/*******************************************************************************
 **
 ** Function         app_hs_stop_voice_recognition
 **
 ** Description      Stop voice recognition
 **
 ** Parameters       None
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_stop_voice_recognition(void)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	printf("app_hs_stop_voice_recognition:Command \n");

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	if(app_hs_cb.conn_cb[0].peer_feature & BSA_HS_PEER_FEAT_VREC)
	{
		BSA_HsCommandInit(&cmd_param);
		cmd_param.hndl = p_conn->handle;
		cmd_param.command = BSA_HS_BVRA_CMD;
		cmd_param.data.num = 0;
		BSA_HsCommand(&cmd_param);
		return 0;
	}

	APP_DEBUG0("Peer feature - VR feature not available");
	return -1;
}

/*******************************************************************************
 **
 ** Function         app_hs_set_volume
 **
 ** Description      Send volume AT Command
 **
 ** Parameters       tBSA_BTHF_VOLUME_TYPE_T type, int volume
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_set_volume(tBSA_BTHF_VOLUME_TYPE_T type, int volume)
{
	tBSA_HS_COMMAND cmd_param;
	tBSA_HS_CONN_CB *p_conn=NULL;

	/* If no connection exist, error */
	if ((NULL==(p_conn = app_hs_get_default_conn())))
	{
		APP_DEBUG0("no connection available");
		return -1;
	}

	BSA_HsCommandInit(&cmd_param);
	cmd_param.hndl = p_conn->handle;

	if(BTHF_VOLUME_TYPE_SPK == type)
		cmd_param.command = BSA_HS_SPK_CMD;
	else
		cmd_param.command = BSA_HS_MIC_CMD;

	cmd_param.data.num = volume;
	BSA_HsCommand(&cmd_param);
	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_getallIndicatorValues
 **
 ** Description      Get all indicator values
 **
 ** Parameters       tBSA_HS_IND_VALS *pIndVals
 **
 ** Returns          0 if successful execution, error code else
 **
 *******************************************************************************/
int app_hs_getallIndicatorValues(tBSA_HS_IND_VALS *pIndVals)
{
	tBSA_HS_CONN_CB * p_conn = &(app_hs_cb.conn_cb[0]);
	if(NULL!=p_conn && NULL!=pIndVals)
	{
		pIndVals->curr_callwait_ind = 0;
		pIndVals->curr_signal_strength_ind = p_conn->curr_signal_strength_ind;
		pIndVals->curr_battery_ind = p_conn->curr_battery_ind;
		pIndVals->curr_call_ind = p_conn->curr_call_ind;
		pIndVals->curr_call_setup_ind = p_conn->curr_call_setup_ind;
		pIndVals->curr_roam_ind = p_conn->curr_roam_ind;
		pIndVals->curr_service_ind = p_conn->curr_service_ind;
		pIndVals->curr_callheld_ind = p_conn->curr_callheld_ind;
		if(pIndVals->curr_call_ind == BSA_HS_CALL_ACTIVE &&
				pIndVals->curr_call_setup_ind == BSA_HS_CALLSETUP_INCOMING)
		{
			pIndVals->curr_callwait_ind = 1;
		}
		return 0;
	}
	return -1;
}


#ifdef PCM_ALSA
/*******************************************************************************
 **
 ** Function         app_hs_open_alsa_duplex
 **
 ** Description      function to open ALSA driver
 **
 ** Parameters       None
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_hs_open_alsa_duplex(void)
{
	int status;

	/* If ALSA PCM driver was already open => close it */
	if (alsa_handle_playback != NULL)
	{
		snd_pcm_close(alsa_handle_playback);
		alsa_handle_playback = NULL;
	}

	APP_DEBUG0("Opening Alsa/Asound audio driver Playback");
	/* Open ALSA driver */
	status = snd_pcm_open(&alsa_handle_playback, alsa_device,
			SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	if (status < 0)
	{
		APP_ERROR1("snd_pcm_open failed: %s", snd_strerror(status));
		return status;
	}
	else
	{
		/* Configure ALSA driver with PCM parameters */
		status = snd_pcm_set_params(alsa_handle_playback,
				SND_PCM_FORMAT_S16_LE,
				SND_PCM_ACCESS_RW_INTERLEAVED,
				APP_HS_CHANNEL_NB,
				APP_HS_SAMPLE_RATE,
				1, /* SW resample */
				100000);/* 100msec */
		if (status < 0)
		{
			APP_ERROR1("snd_pcm_set_params failed: %s", snd_strerror(status));
			return status;
		}
	}
	alsa_playback_opened = TRUE;

	/* If ALSA PCM driver was already open => close it */
	if (alsa_handle_capture != NULL)
	{
		snd_pcm_close(alsa_handle_capture);
		alsa_handle_capture = NULL;
	}
	APP_DEBUG0("Opening Alsa/Asound audio driver Capture");

	status = snd_pcm_open(&alsa_handle_capture, alsa_device,
			SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
	if (status < 0)
	{
		APP_ERROR1("snd_pcm_open failed: %s", snd_strerror(status));
		return status;
	}
	else
	{
		/* Configure ALSA driver with PCM parameters */
		status = snd_pcm_set_params(alsa_handle_capture,
				SND_PCM_FORMAT_S16_LE,
				SND_PCM_ACCESS_RW_INTERLEAVED,
				APP_HS_CHANNEL_NB,
				APP_HS_SAMPLE_RATE,
				1, /* SW resample */
				100000);/* 100msec */
		if (status < 0)
		{
			APP_ERROR1("snd_pcm_set_params failed: %s", snd_strerror(status));
			return status;
		}
	}
	alsa_capture_opened = TRUE;

	return 0;
}

/*******************************************************************************
 **
 ** Function         app_hs_close_alsa_duplex
 **
 ** Description      function to close ALSA driver
 **
 ** Parameters       None
 **
 ** Returns          void
 **
 *******************************************************************************/
int app_hs_close_alsa_duplex(void)
{
	if (alsa_handle_playback != NULL)
	{
		snd_pcm_close(alsa_handle_playback);
		alsa_handle_playback = NULL;
		alsa_playback_opened = FALSE;
	}
	if (alsa_handle_capture != NULL)
	{
		snd_pcm_close(alsa_handle_capture);
		alsa_handle_capture = NULL;
		alsa_capture_opened = FALSE;
	}
	return 0;
}
#endif

