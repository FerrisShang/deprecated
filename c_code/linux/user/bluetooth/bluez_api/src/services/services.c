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
