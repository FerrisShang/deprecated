/*****************************************************************************
 **
 **  Name:           app_ops_main.c
 **
 **  Description:    Bluetooth Manager application
 **
 **  Copyright (c) 2010-2013, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/
#include <stdlib.h>
#include <unistd.h>

#include "app_ops.h"
#include "app_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_xml_utils.h"

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_ops_start
 **
 ** Description      start OPP Server application
 **
 ** Parameters       void
 **
 ** Returns          BSA_SUCCESS success, error code for failure
 **
 *******************************************************************************/
tBSA_STATUS mozart_bluetooth_ops_start(void)
{
	tBSA_STATUS status = 0;

	/* start OPS application */
	status = app_start_ops();
	if (status != BSA_SUCCESS)
	{
		printf("main: Unable to start OPS\n");
		return status;
	}
	return status;
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_ops_stop
 **
 ** Description      stop OPP Server application
 **
 ** Parameters       void
 **
 ** Returns          BSA_SUCCESS success, error code for failure
 **
 *******************************************************************************/
tBSA_STATUS mozart_bluetooth_ops_stop()
{
	/* example to stop OPC service */
	return app_stop_ops();
}

/*******************************************************************************
 **
 ** Function         mozart_bluetooth_ops_auto_accept
 **
 ** Description      set ops auto accept object
 **
 ** Parameters       void
 **
 ** Returns          BSA_SUCCESS success, error code for failure
 **
 *******************************************************************************/
tBSA_STATUS mozart_bluetooth_ops_auto_accept(void)
{
	return app_ops_auto_accept();
}
/*******************************************************************************
 **
 ** Function         app_ops_display_main_menu
 **
 ** Description      This is the main menu
 **
 ** Parameters      void
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ops_display_main_menu(void)
{
	printf("\nBluetooth Object Push Server Running\n");
	printf("    1 => enable auto accept\n");
	printf("    2 => disconnect device\n");
	printf("    9 => quit\n");
}
