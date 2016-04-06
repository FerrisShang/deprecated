#include <stdio.h>
#include "app_disc.h"

extern int bsa_disc_device_count;

int mozart_bluetooth_disc_start_regular(void)
{
	return app_disc_start_regular(NULL);
}

int mozart_bluetooth_disc_get_new_device_number(void)
{
	return bsa_disc_device_count;
}
