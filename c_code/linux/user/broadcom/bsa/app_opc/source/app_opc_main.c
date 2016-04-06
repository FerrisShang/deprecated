/*****************************************************************************
 **
 **  Name:           app_opc_main.c
 **
 **  Description:    Bluetooth Manager application
 **
 **  Copyright (c) 2010-2013, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/
#include <stdlib.h>
#include <unistd.h>

#include "app_opc.h"
#include "app_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_xml_utils.h"

#define APP_TEST_FILE_PATH "/usr/data/test_file.txt"

/* ui keypress definition */
#define APP_OPC_KEY_PUSH        1
#define APP_OPC_KEY_EXCH        2
#define APP_OPC_KEY_PULL        3
#define APP_OPC_KEY_PUSH_FILE   4
#define APP_OPC_KEY_CLOSE       5
#define APP_OPC_KEY_DISC        6
#define APP_OPC_KEY_QUIT        9

/*******************************************************************************
 **
 ** Function         app_opc_display_main_menu
 **
 ** Description      This is the main menu
 **
 ** Parameters      void
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_opc_display_main_menu(void)
{
	printf("\nBluetooth Object Push Server Running\n");
	printf("    %d => push vcard\n", APP_OPC_KEY_PUSH);
	printf("    %d => exchange vcard\n", APP_OPC_KEY_EXCH);
	printf("    %d => pull vcard\n", APP_OPC_KEY_PULL);
	printf("    %d => push %s\n", APP_OPC_KEY_PUSH_FILE, APP_TEST_FILE_PATH);
	printf("    %d => close\n", APP_OPC_KEY_CLOSE);
	printf("    %d => discovery\n", APP_OPC_KEY_DISC);
	printf("    %d => quit\n", APP_OPC_KEY_QUIT);
}

/*******************************************************************************
 **
 ** function         mozart_bluetooth_opc_start
 **
 ** description      start opp client application
 **
 ** parameters       void
 **
 ** returns          bsa_success success, error code for failure
 **
 *******************************************************************************/
tBSA_STATUS mozart_bluetooth_opc_start(void)
{
	tBSA_STATUS status;

	/* Example of function to start OPC application */
	status = app_opc_start();
	if (status != BSA_SUCCESS)
	{
		fprintf(stderr, "main: Unable to start OPC\n");
		app_mgt_close();
		return status;
	}
	return status;
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_opc_stop
 **
 ** Description      stop OPP Client application
 **
 ** Parameters       void
 **
 ** Returns          BSA_SUCCESS success, error code for failure
 **
 *******************************************************************************/
tBSA_STATUS mozart_bluetooth_opc_stop(void)
{
	/* example to stop OPC service */
	return app_opc_stop();
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_opc_set_key
 **
 ** Description      Select key to set opc Function
 **
 ** Parameters       APP_OPC_KEY_XXX
 **
 ** Returns          BSA_SUCCESS success, error code for failure
 **
 *******************************************************************************/
tBSA_STATUS mozart_bluetooth_opc_set_key(int choice)
{
	tBSA_STATUS status;
	switch (choice)
	{
		case APP_OPC_KEY_PUSH:
			status = app_opc_push_vc();
			break;
		case APP_OPC_KEY_EXCH:
			status = app_opc_exchange_vc();
			break;
		case APP_OPC_KEY_PULL:
			status = app_opc_pull_vc();
			break;
		case APP_OPC_KEY_PUSH_FILE:
			status = app_opc_push_file(APP_TEST_FILE_PATH);
			break;
		case APP_OPC_KEY_CLOSE:
			status = app_opc_disconnect();
			break;
		case APP_OPC_KEY_DISC:
			/* Example to perform Device discovery (in blocking mode) */
			status = app_disc_start_regular(NULL);
			break;
		case APP_OPC_KEY_QUIT:
			break;
		default:
			printf("main: Unknown choice:%d\n", choice);
			break;
	}

	return status;
}
