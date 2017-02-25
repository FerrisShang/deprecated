/*
 * services.c
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co., Ltd
 * Author: Feng Shang <feng.shang@ingenic.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

extern void init_automation_io(void);
extern void init_battery(void);
extern void init_current_time(void);
extern void init_device_info(void);
extern void init_generic_access(void);
extern void init_heart_rate(void);
extern void init_immediate_alert(void);
extern void init_phone_alert(void);
extern void init_service_changed(void);
extern void init_running_speed(void);
extern void init_tx_power(void);
extern void init_uesr_data(void);
extern void init_hid_device(void);
extern void init_generic_attribute(void);

void init_gatt_services(void)
{
	init_hid_device();
	init_generic_access();
}
