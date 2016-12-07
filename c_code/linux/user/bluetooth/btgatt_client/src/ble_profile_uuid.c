#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "lib/bluetooth.h"
#include "lib/uuid.h"
#include "ble_profile_uuid.h"

struct service_name{
	bt_uuid_t uuid;
	char name[80];
};
#define DEFINE_UUID16(uuid) {BT_UUID16, .value.u16 = (uuid)}
static struct service_name service_name[] = {
	//service
	{ DEFINE_UUID16(0x1811), "Automation_IO(0x1811)" },
	{ DEFINE_UUID16(0x1815), "Battery Service(0x1815)" },
	{ DEFINE_UUID16(0x180F), "Blood Pressure(0x180F)" },
	{ DEFINE_UUID16(0x1810), "Body Composition(0x1810)" },
	{ DEFINE_UUID16(0x181B), "Bond Management(0x181B)" },
	{ DEFINE_UUID16(0x181E), "Continuous Glucose Monitoring(0x181E)" },
	{ DEFINE_UUID16(0x1805), "Current Time Service(0x1805)" },
	{ DEFINE_UUID16(0x1818), "Cycling Power(0x1818)" },
	{ DEFINE_UUID16(0x1816), "Cycling Speed and Cadence(0x1816)" },
	{ DEFINE_UUID16(0x180A), "Device Information(0x180A)" },
	{ DEFINE_UUID16(0x181A), "Environmental Sensing(0x181A)" },
	{ DEFINE_UUID16(0x1800), "Generic Access(0x1800)" },
	{ DEFINE_UUID16(0x1801), "Generic Attribute(0x1801)" },
	{ DEFINE_UUID16(0x1808), "Glucose(0x1808)" },
	{ DEFINE_UUID16(0x1809), "Health Thermometer(0x1809)" },
	{ DEFINE_UUID16(0x180D), "Heart Rate(0x180D)" },
	{ DEFINE_UUID16(0x1823), "HTTP Proxy(0x1823)" },
	{ DEFINE_UUID16(0x1812), "Human Interface Device(0x1812)" },
	{ DEFINE_UUID16(0x1802), "Immediate Alert(0x1802)" },
	{ DEFINE_UUID16(0x1821), "Indoor Positioning(0x1821)" },
	{ DEFINE_UUID16(0x1820), "Internet Protocol Support(0x1820)" },
	{ DEFINE_UUID16(0x1803), "Link Loss(0x1803)" },
	{ DEFINE_UUID16(0x1819), "Location and Navigation(0x1819)" },
	{ DEFINE_UUID16(0x1807), "Next DST Change Service(0x1807)" },
	{ DEFINE_UUID16(0x1825), "Object Transfer(0x1825)" },
	{ DEFINE_UUID16(0x180E), "Phone Alert Status Service(0x180E)" },
	{ DEFINE_UUID16(0x1822), "Pulse Oximeter(0x1822)" },
	{ DEFINE_UUID16(0x1806), "Reference Time Update Service(0x1806)" },
	{ DEFINE_UUID16(0x1814), "Running Speed and Cadence(0x1814)" },
	{ DEFINE_UUID16(0x1813), "Scan Parameters(0x1813)" },
	{ DEFINE_UUID16(0x1824), "Transport Discovery(0x1824)" },
	{ DEFINE_UUID16(0x1804), "Tx Power(0x1804)" },
	{ DEFINE_UUID16(0x181C), "User Data(0x181C)" },
	{ DEFINE_UUID16(0x181D), "Weight Scale(0x181D)" },
	//characteristic
	{ DEFINE_UUID16(0x2A56), "Digital(0x2A56)" },
	{ DEFINE_UUID16(0x2A58), "Analog(0x2A58)" },
	{ DEFINE_UUID16(0x2A5A), "Aggregate(0x2A5A)" },
	{ DEFINE_UUID16(0x2A19), "Battery Level(0x2A19)" },
	{ DEFINE_UUID16(0x2A2B), "Current Time(0x2A2B)" },
	{ DEFINE_UUID16(0x2A0F), "Local Time Information(0x2A0F)" },
	{ DEFINE_UUID16(0x2A14), "Reference Time Information(0x2A14)" },
	{ DEFINE_UUID16(0x2A29), "Manufacturer Name String(0x2A29)" },
	{ DEFINE_UUID16(0x2A24), "Model Number String(0x2A24)" },
	{ DEFINE_UUID16(0x2A25), "Serial Number String(0x2A25)" },
	{ DEFINE_UUID16(0x2A27), "Hardware Revision String(0x2A27)" },
	{ DEFINE_UUID16(0x2A26), "Firmware Revision String(0x2A26)" },
	{ DEFINE_UUID16(0x2A28), "Software Revision String(0x2A28)" },
	{ DEFINE_UUID16(0x2A23), "System ID(0x2A23)" },
	{ DEFINE_UUID16(0x2A50), "PnP ID(0x2A50)" },
	{ DEFINE_UUID16(0x2A00), "Device Name(0x2A00)" },
	{ DEFINE_UUID16(0x2A01), "Appearance(0x2A01)" },
	{ DEFINE_UUID16(0x2A02), "Peripheral Privacy Flag(0x2A02)" },
	{ DEFINE_UUID16(0x2A03), "Reconnection Address(0x2A03)" },
	{ DEFINE_UUID16(0x2A04), "Peripheral Preferred Connection Parameters(0x2A04)" },
	{ DEFINE_UUID16(0x2A06), "Alert Level(0x2A06)" },
	{ DEFINE_UUID16(0x2A53), " RSC Measurement(0x2A53)" },
	{ DEFINE_UUID16(0x2A54), "RSC Feature(0x2A54)" },
	{ DEFINE_UUID16(0x2A5D), "Sensor Location(0x2A5D)" },
	{ DEFINE_UUID16(0x2A55), "SC Control Point(0x2A55)" },
	{ DEFINE_UUID16(0x2A07), "Tx Power Level(0x2A07)" },
	{ DEFINE_UUID16(0x2A8A), "First Name(0x2A8A)" },
	{ DEFINE_UUID16(0x2A90), "Last Name(0x2A90)" },
	{ DEFINE_UUID16(0x2A87), "Email Address(0x2A87)" },
	{ DEFINE_UUID16(0x2A80), "Age(0x2A80)" },
	{ DEFINE_UUID16(0x2A85), "Date of Birth(0x2A85)" },
	{ DEFINE_UUID16(0x2A8C), "Gender(0x2A8C)" },
	{ DEFINE_UUID16(0x2A98), "Weight(0x2A98)" },
	{ DEFINE_UUID16(0x2A37), "Heart Rate Measurement(0x2A37)" },
	{ DEFINE_UUID16(0x2A38), "Body Sensor Location(0x2A38)" },
	{ DEFINE_UUID16(0x2A39), "Heart Rate Control Point(0x2A39)" },
	{ DEFINE_UUID16(0x2A3F), "Alert Status(0x2A3F)" },
	{ DEFINE_UUID16(0x2A41), "Ringer Setting(0x2A41)" },
	{ DEFINE_UUID16(0x2A40), "Ringer Control point(0x2A40)" },
};

#define BT_GATT_CHRC_PROP_BROADCAST          0x01
#define BT_GATT_CHRC_PROP_READ               0x02
#define BT_GATT_CHRC_PROP_WRITE_WITHOUT_RESP 0x04
#define BT_GATT_CHRC_PROP_WRITE              0x08
#define BT_GATT_CHRC_PROP_NOTIFY             0x10
#define BT_GATT_CHRC_PROP_INDICATE           0x20
#define BT_GATT_CHRC_PROP_AUTH               0x40
#define BT_GATT_CHRC_PROP_EXT_PROP           0x80


void get_props_str(char *props_str, int props)
{
	props_str[0] = '\0';
	if(props == 0){
		strcat(props_str, "No Permission");
		return;
	}
	if(props & BT_GATT_CHRC_PROP_BROADCAST){
		strcat(props_str, "Broadcast ");
	}
	if(props & BT_GATT_CHRC_PROP_READ){
		strcat(props_str, "Read ");
	}
	if(props & BT_GATT_CHRC_PROP_WRITE_WITHOUT_RESP){
		strcat(props_str, "Write without response");
	}
	if(props & BT_GATT_CHRC_PROP_WRITE){
		strcat(props_str, "Write ");
	}
	if(props & BT_GATT_CHRC_PROP_NOTIFY){
		strcat(props_str, "Notify ");
	}
	if(props & BT_GATT_CHRC_PROP_INDICATE){
		strcat(props_str, "Indicate ");
	}
	if(props & BT_GATT_CHRC_PROP_AUTH){
		strcat(props_str, "Authenticated Signed Writes ");
	}
	if(props & BT_GATT_CHRC_PROP_EXT_PROP){
		strcat(props_str, "Extended Properties ");
	}

}




/*
   bt_uuid_t UUID_ALERT_NOTIFICATION            = DEFINE_UUID16(0x1811);
   bt_uuid_t UUID_AUTOMATION_IO                 = DEFINE_UUID16(0x1815);
   bt_uuid_t UUID_BATTERY_SERVICE               = DEFINE_UUID16(0x180F);
   bt_uuid_t UUID_BLOOD_PRESSURE                = DEFINE_UUID16(0x1810);
   bt_uuid_t UUID_BODY_COMPOSITION              = DEFINE_UUID16(0x181B);
   bt_uuid_t UUID_BOND_MANAGEMENT               = DEFINE_UUID16(0x181E);
   bt_uuid_t UUID_CURRENT_TIME_SERVICE          = DEFINE_UUID16(0x1805);
   bt_uuid_t UUID_CYCLING_POWER                 = DEFINE_UUID16(0x1818);
   bt_uuid_t UUID_CYCLING_SPEED_AND_CADENCE     = DEFINE_UUID16(0x1816);
   bt_uuid_t UUID_DEVICE_INFORMATION            = DEFINE_UUID16(0x180A);
   bt_uuid_t UUID_ENVIRONMENTAL_SENSING         = DEFINE_UUID16(0x181A);
   bt_uuid_t UUID_GENERIC_ACCESS                = DEFINE_UUID16(0x1800);
   bt_uuid_t UUID_GENERIC_ATTRIBUTE             = DEFINE_UUID16(0x1801);
   bt_uuid_t UUID_GLUCOSE                       = DEFINE_UUID16(0x1808);
   bt_uuid_t UUID_HEALTH_THERMOMETER            = DEFINE_UUID16(0x1809);
   bt_uuid_t UUID_HEART_RATE                    = DEFINE_UUID16(0x180D);
   bt_uuid_t UUID_HTTP_PROXY                    = DEFINE_UUID16(0x1823);
   bt_uuid_t UUID_HUMAN_INTERFACE_DEVICE        = DEFINE_UUID16(0x1812);
   bt_uuid_t UUID_IMMEDIATE_ALERT               = DEFINE_UUID16(0x1802);
   bt_uuid_t UUID_INDOOR_POSITIONING            = DEFINE_UUID16(0x1821);
   bt_uuid_t UUID_INTERNET_PROTOCOL_SUPPORT     = DEFINE_UUID16(0x1820);
   bt_uuid_t UUID_LINK_LOSS                     = DEFINE_UUID16(0x1803);
   bt_uuid_t UUID_LOCATION_AND_NAVIGATION       = DEFINE_UUID16(0x1819);
   bt_uuid_t UUID_NEXT_DST_CHANGE_SERVICE       = DEFINE_UUID16(0x1807);
   bt_uuid_t UUID_OBJECT_TRANSFER               = DEFINE_UUID16(0x1825);
   bt_uuid_t UUID_PHONE_ALERT_STATUS_SERVICE    = DEFINE_UUID16(0x180E);
   bt_uuid_t UUID_PULSE_OXIMETER                = DEFINE_UUID16(0x1822);
   bt_uuid_t UUID_REFERENCE_TIME_UPDATE_SERVICE = DEFINE_UUID16(0x1806);
   bt_uuid_t UUID_RUNNING_SPEED_AND_CADENCE     = DEFINE_UUID16(0x1814);
   bt_uuid_t UUID_SCAN_PARAMETERS               = DEFINE_UUID16(0x1813);
   bt_uuid_t UUID_TRANSPORT_DISCOVERY           = DEFINE_UUID16(0x1824);
   bt_uuid_t UUID_TX_POWER                      = DEFINE_UUID16(0x1804);
   bt_uuid_t UUID_USER_DATA                     = DEFINE_UUID16(0x181C);
   bt_uuid_t UUID_WEIGHT_SCALE                  = DEFINE_UUID16(0x181D);
   */

char* get_UUID_str(bt_uuid_t *uuid)
{
	int i;
	for(i=0;i<sizeof(service_name)/sizeof(service_name[0]);i++){
		if(!bt_uuid_cmp(uuid , &service_name[i].uuid)){
			return service_name[i].name;
		}
	}
	return NULL;
}

