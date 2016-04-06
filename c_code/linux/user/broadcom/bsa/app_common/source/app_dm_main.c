#include <stdio.h>
#include "app_dm.h"

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_set_visibility
 **
 ** Description     Set the Device Visibility parameters (InquiryScan & PageScan)
 **
 ** Parameters      discoverable: FALSE if not discoverable
 **                 connectable: FALSE if not connectable
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_set_visibility(BOOLEAN discoverable, BOOLEAN connectable)
{
	return app_dm_set_visibility(discoverable, connectable);
}

/*******************************************************************************
 **
 ** Function        mozart_bluetooth_ble_set_visibility
 **
 ** Description     Set the Device BLE Visibility parameters
 **
 ** Parameters      discoverable: FALSE if not discoverable
 **                 connectable: FALSE if not connectable
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int mozart_bluetooth_ble_set_visibility(BOOLEAN discoverable, BOOLEAN connectable)
{
	return app_dm_set_ble_visibility(discoverable, connectable);
}
