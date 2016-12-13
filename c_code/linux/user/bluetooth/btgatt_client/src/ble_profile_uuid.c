#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "lib/bluetooth.h"
#include "lib/uuid.h"
#include "src/shared/queue.h"
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
	{ DEFINE_UUID16(0x2A05), "Service Changed(0x2A05)" },
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
	{ DEFINE_UUID16(0x2A8E), "Height(0x2A8E)" },
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

static struct queue *queue_chrc_handle;
typedef struct {
	bt_uuid_t uuid;
	uint16_t handle;
}chrc_handle_t;

void clear_chrc_handle(void)
{
	if(queue_chrc_handle == NULL){
		queue_chrc_handle = queue_new();
	}
	queue_remove_all(queue_chrc_handle, NULL, NULL, free);
}

static bool chrc_handle_match_cb(const void *data, const void *match_data)
{
	chrc_handle_t *entry = (chrc_handle_t*)data;
	uint16_t *handle = (uint16_t*)match_data;
	return (entry->handle == *handle);
}

int get_chrc_by_handle(bt_uuid_t *uuid, uint16_t handle)
{
	chrc_handle_t *data;
	if(queue_chrc_handle == NULL){
		clear_chrc_handle();
	}
	data = queue_find(queue_chrc_handle, chrc_handle_match_cb, &handle);
	if(data){
		memcpy(uuid, &data->uuid, sizeof(bt_uuid_t));
		return 0;
	}
	return -1;
}
int put_chrc_by_handle(bt_uuid_t *uuid, uint16_t handle)
{
	chrc_handle_t *data;
	if(queue_chrc_handle == NULL){
		clear_chrc_handle();
	}
	data = malloc(sizeof(chrc_handle_t));
	memcpy(&data->uuid, uuid, sizeof(bt_uuid_t));
	data->handle = handle;
	queue_push_tail(queue_chrc_handle, data);
	return 0;
}
static int uuid16_cmp(bt_uuid_t *uuid, uint16_t uuid16){
	bt_uuid_t tmp = DEFINE_UUID16(uuid16);
	return bt_uuid_cmp(&tmp, uuid);
}
char* conv_data_to_str(bt_uuid_t *uuid, char *buf, const uint8_t *value, uint16_t length)
{
	buf[0] = 0;
	int i;
	for(i=0;i<sizeof(service_name)/sizeof(service_name[0]);i++){
		if(!uuid16_cmp(uuid , 0x2A56)){ //Digital
			char *digital_state[] = {"Inactive", "Active", "Tri-state", "Output-state"};
			int key = value[0];
			if(key > sizeof(digital_state)/sizeof(digital_state[0])){
				strcpy(buf, "Digital state Unknown");
			}else{
				sprintf(buf, "Digital state : %s", digital_state[key]);
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A58)){ //Analog
			sprintf(buf, "Analog value : 0x%04x", value[0] + (value[1]<<8));
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A5A)){ //Aggregate
			char *digital_state[] = {"Inactive", "Active", "Tri-state", "Output-state"};
			int key = value[0];
			if(key > sizeof(digital_state)/sizeof(digital_state[0])){
				strcpy(buf, "Digital state Unknown");
			}else{
				sprintf(buf, "Digital state : %s", digital_state[key]);
			}
			sprintf(buf, "%s  Analog value : 0x%04x", buf, value[0] + (value[1]<<8));
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A19)){ //Battery Level
			int level = value[0];
			sprintf(buf, "Level : %d%%", level);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A2B)){ //Current Time
			char *week[] = {"Unknown", "Monday", "Tuesday", "Wednesday",
				"Thursday", "Friday", " Saturday", "Sunday"};
			int year,mon,day,hour,min,sec,wek, min_sec;
			int adjust_reason;
			UINT8 *p = (UINT8*)value;
			STREAM_TO_UINT16(year, p);
			STREAM_TO_UINT8(mon, p);
			STREAM_TO_UINT8(day, p);
			STREAM_TO_UINT8(hour, p);
			STREAM_TO_UINT8(min, p);
			STREAM_TO_UINT8(sec, p);
			STREAM_TO_UINT8(wek, p);
			if(wek > sizeof(week)/sizeof(week[0])){
				wek = 0;
			}
			STREAM_TO_UINT8(min_sec, p);
			min_sec = (int)((float)min_sec/256.0*1000.0);
			sprintf(buf, "%d-%d-%d  %d:%02d:%02d.%03d %s",
					year, mon, day, hour, min, sec, min_sec, week[wek]);
			STREAM_TO_UINT8(adjust_reason, p);
			if(adjust_reason){
				strcat(buf, "\nAdjust Reason : ");
			}
			if(adjust_reason & (1<<0)){
				strcat(buf, "Manual time update. ");
			}
			if(adjust_reason & (1<<1)){
				strcat(buf, "External reference time update. ");
			}
			if(adjust_reason & (1<<2)){
				strcat(buf, "Change of time zone. ");
			}
			if(adjust_reason & (1<<3)){
				strcat(buf, "Change of DST (daylight savings time). ");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A0F)){ //Local Time Information
			static struct {char key; char *zone;}
			zone_str[] = {
				{-48, "UTC-12:00"}, {-44, "UTC-11:00"}, {-40, "UTC-10:00"}, {-38, "UTC-9:30"},
				{-36, "UTC-9:00"},  {-32, "UTC-8:00"},  {-28, "UTC-7:00"},  {-24, "UTC-6:00"},
				{-20, "UTC-5:00"},  {-18, "UTC-4:30"},  {-16, "UTC-4:00"},  {-14, "UTC-3:30"},
				{-12, "UTC-3:00"},  {-8, "UTC-2:00"},   {-4, "UTC-1:00"},   {0, "UTC+0:00"},
				{4, "UTC+1:00"},    {8, "UTC+2:00"},    {12, "UTC+3:00"},   {14, "UTC+3:30"},
				{16, "UTC+4:00"},   {18, "UTC+4:30"},   {20, "UTC+5:00"},   {22, "UTC+5:30"},
				{23, "UTC+5:45"},   {24, "UTC+6:00"},   {26, "UTC+6:30"},   {28, "UTC+7:00"},
				{32, "UTC+8:00"},   {35, "UTC+8:45"},   {36, "UTC+9:00"},   {38, "UTC+9:30"},
				{40, "UTC+10:00"},  {42, "UTC+10:30"},  {44, "UTC+11:00"},  {46, "UTC+11:30"},
				{48, "UTC+12:00"},  {51, "UTC+12:45"},  {52, "UTC+13:00"},  {56, "UTC+14:00"},
			};
			char dst_offset, time_zone;
			int i;
			UINT8 *p = (UINT8*)value;
			strcpy(buf, "Time zone offset is not known.");
			STREAM_TO_UINT8(time_zone, p);
			STREAM_TO_UINT8(dst_offset, p);
			for(i=0;i<sizeof(zone_str)/sizeof(zone_str[0]);i++){
				if(zone_str[i].key == time_zone){
					strcpy(buf, zone_str[i].zone);
				}
			}
			strcat(buf, "  ");
			if(dst_offset == 0){
				strcat(buf, "Standard Time");
			}else if(dst_offset == 2){
				strcat(buf, "Half An Hour Daylight Time (+0.5h)");
			}else if(dst_offset == 4){
				strcat(buf, "Daylight Time (+1h)");
			}else if(dst_offset == 8){
				strcat(buf, "Double Daylight Time (+2h)");
			}else{
				strcat(buf, "DST is not known");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A14)){ //Reference Time Information
			static struct {int key; char *value;}
			time_source[] = {
				{0, "Unknown"},
				{1, "Network Time Protocol"},
				{2, "GPS"},
				{3, "Radio Time Signal"},
				{4, "Manual"},
				{5, "Atomic Clock"},
				{6, "Cellular Network"},
			};
			UINT8 source, accuracy, days_since_update, hours_since_update;
			UINT8 i, *p = (UINT8*)value;
			STREAM_TO_UINT8(source, p);
			STREAM_TO_UINT8(accuracy, p);
			STREAM_TO_UINT8(days_since_update, p);
			STREAM_TO_UINT8(hours_since_update, p);
			if(source >= sizeof(time_source)/sizeof(time_source[0])){
				source = 0;
			}
			strcpy(buf, "Time Source : ");
			for(i=0;i<sizeof(time_source)/sizeof(time_source[0]);i++){
				if(time_source[i].key == source){
					strcat(buf, time_source[i].value);
				}
			}
			if(accuracy == 254){
				strcat(buf, "  Accuracy out of range.");
			}else if(accuracy == 255){
				strcat(buf, "  Accuracy Unknown.");
			}else{
				sprintf(buf, "%s  Accuracy : %.1fs", buf, (float)accuracy/8.0);
			}
			strcat(buf, "\n");
			sprintf(buf, "%sDays Since Update:%d  Hours Since Update:%d",
					buf, days_since_update, hours_since_update);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A29)){ //Manufacturer Name String
			strcpy(buf, "Manufacturer Name : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A24)){ //Model Number String
			strcpy(buf, "Model Number : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A25)){ //Serial Number String
			strcpy(buf, "MSerial Number : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A27)){ //Hardware Revision String
			strcpy(buf, "Hardware Revision : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A26)){ //Firmware Revision String
			strcpy(buf, "Firmware Revision : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A28)){ //Software Revision String
			strcpy(buf, "Software Revision : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A23)){ //System ID
		}else if(!uuid16_cmp(uuid , 0x2A50)){ //PnP ID
		}else if(!uuid16_cmp(uuid , 0x2A00)){ //Device Name
			strcpy(buf, "Device name : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A01)){ //Appearance
			static struct {int key; char *value; char *description;}
			appearance[] = {
				{0, "Unknown", "None"},
				{64, "Generic Phone", "Generic category"},
				{128, "Generic Computer", "Generic category"},
				{192, "Generic Watch", "Generic category"},
				{193, "Watch: Sports Watch", "Watch subtype"},
				{256, "Generic Clock", "Generic category"},
				{320, "Generic Display", "Generic category"},
				{384, "Generic Remote Control", "Generic category"},
				{448, "Generic Eye-glasses", "Generic category"},
				{512, "Generic Tag", "Generic category"},
				{576, "Generic Keyring", "Generic category"},
				{640, "Generic Media Player", "Generic category"},
				{704, "Generic Barcode Scanner", "Generic category"},
				{768, "Generic Thermometer", "Generic category"},
				{769, "Thermometer: Ear", "Thermometer subtype"},
				{832, "Generic Heart rate Sensor", "Generic category"},
				{833, "Heart Rate Sensor: Heart Rate Belt", "Heart Rate Sensor subtype"},
				{896, "Generic Blood Pressure", "Generic category"},
				{897, "Blood Pressure: Arm", "Blood Pressure subtype"},
				{898, "Blood Pressure: Wrist", "Blood Pressure subtype"},
				{960, "Human Interface Device (HID)", "HID Generic"},
				{961, "Keyboard", "HID subtype"},
				{962, "Mouse", "HID subtype"},
				{963, "Joystick", "HID subtype"},
				{964, "Gamepad", "HID subtype"},
				{965, "Digitizer Tablet", "HID subtype"},
				{966, "Card Reader", "HID subtype"},
				{967, "Digital Pen", "HID subtype"},
				{968, "Barcode Scanner", "HID subtype"},
				{1024, "Generic Glucose Meter", "Generic category"},
				{1088, "Generic: Running Walking Sensor", "Generic category"},
				{1089, "Running Walking Sensor: In-Shoe", "Running Walking Sensor subtype"},
				{1090, "Running Walking Sensor: On-Shoe", "Running Walking Sensor subtype"},
				{1091, "Running Walking Sensor: On-Hip", "Running Walking Sensor subtype"},
				{1152, "Generic: Cycling", "Generic category"},
				{1153, "Cycling: Cycling Computer", "Cycling subtype"},
				{1154, "Cycling: Speed Sensor", "Cycling subtype"},
				{1155, "Cycling: Cadence Sensor", "Cycling subtype"},
				{1156, "Cycling: Power Sensor", "Cycling subtype"},
				{1157, "Cycling: Speed and Cadence Sensor", "Cycling subtype"},
				{3136, "Generic: Pulse Oximeter", "Pulse Oximeter Generic Gategory"},
				{3137, "Fingertip", "Pulse Oximeter subtype"},
				{3138, "Wrist Worn", "Pulse Oximeter subtype"},
				{3200, "Generic: Weight Scale", "Weight Scale Generic Category"},
				{5184, "Generic: Outdoor Sports Activity", "Outdoor Sports Activity Generic Category"},
				{5185, "Location Display Device", "Outdoor Sports Activity subtype"},
				{5186, "Location and Navigation Display Device", "Outdoor Sports Activity subtype"},
				{5187, "Location Pod", "Outdoor Sports Activity subtype"},
				{5188, "Location and Navigation Pod", "Outdoor Sports Activity subtype"},
			};
			int i=0, key = value[0] + (value[1]<<8);
			sprintf(buf, "Category : %s(%s)", appearance[0].value, appearance[0].description);
			for(i=0;i<sizeof(appearance)/sizeof(appearance[0]);i++){
				if(appearance[i].key == key){
					sprintf(buf, "Category : %s(%s)", appearance[i].value, appearance[i].description);
				}
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A02)){ //Peripheral Privacy Flag
			int flag = value[0];
			if(flag == 0){
				strcpy(buf, "privacy is disabled in this device");
			}else{
				strcpy(buf, "privacy is enabled in this device");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A03)){ //Reconnection Address
			sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
					value[5], value[4], value[3], value[2], value[1], value[0]);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A04)){ //Peripheral Preferred Connection Parameters
			int min = value[0] + (value[1]<<8);
			int max = value[2] + (value[3]<<8);
			int latency = value[4] + (value[5]<<8);
			int timeout = value[6] + (value[7]<<8);
			sprintf(buf, "Min Interval:%d*1.25ms, Max Interval:%d*1.25m\n"
					"Slave Latency:%d, Supervision Timeout:%d",
					min,max,latency,timeout);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A05)){ //Service Changed
			int start = value[0] + (value[1] << 8);
			int end = value[2] + (value[3] << 8);
			sprintf(buf, "handle : %d ~ %d", start, end);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A06)){ //Alert Level
			int status = value[0];
			if(status == 1){
				strcpy(buf, "Mild Alert");
			}else if(status == 2){
				strcpy(buf, "High Alert");
			}else{
				strcpy(buf, "No Alert");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A53)){ //RSC Measurement
			char flag, instantaneous_cadence;
			uint16_t instantaneous_speed, instantaneous_stride_length;
			uint32_t total_distance;
			UINT8 *p = (UINT8*)value;
			STREAM_TO_UINT8(flag, p);
			STREAM_TO_UINT16(instantaneous_speed, p);
			STREAM_TO_UINT8(instantaneous_cadence, p);
			STREAM_TO_UINT16(instantaneous_stride_length, p);
			STREAM_TO_UINT8(total_distance, p);
			sprintf(buf, "Instantaneous Speed:%.1fm/s  Instantaneous Cadencei:%dRPM  ",
					(float)instantaneous_speed/256.0, instantaneous_cadence);
			if(flag & (1<<2)){
				strcat(buf, "Status:Running");
			}else{
				strcat(buf, "Status:Walking");
			}
			if(flag & (1<<0)){
				sprintf(buf, "\t%s\nInstantaneous Stride Length : %.1f m",
						buf, (float)instantaneous_stride_length/100.0);
			}
			if(flag & (1<<1)){
				sprintf(buf, "\t%s\nTotal Distance: %.1f m",
						buf, (float)total_distance/10.0);
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A54)){ //RSC Feature
			char *st[] = {"False","True"};
			int feature = value[0];
			sprintf(buf, "Instantaneous Stride Length Measurement Supported : %s", st[feature & (1<<0)]);
			sprintf(buf, "%s\nTotal Distance Measurement Supporte : %s", buf, st[feature & (1<<0)]);
			sprintf(buf, "%s\nWalking or Running Status Supporte : %s", buf, st[feature & (1<<0)]);
			sprintf(buf, "%s\nCalibration Procedure Supporte : %s", buf, st[feature & (1<<0)]);
			sprintf(buf, "%s\nMultiple Sensor Locations Supporte : %s", buf, st[feature & (1<<0)]);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A5D)){ //Sensor Location
			const char *location_str[] = {
				"Other", "Top of shoe", "In shoe", "Hip", "Front Wheel",
				"Left Crank", "Right Crank", "Left Pedal", "Right Pedal", "Front Hub",
				"Rear Dropout", "Chainstay", "Rear Wheel", "Rear Hub", "Chest",
				"Spider", "Chain Ring"};
			int location = value[0];
			if(location >= sizeof(location_str)/sizeof(location_str[0])){
				location = 0; //"other"
			}
			strcpy(buf, location_str[location]);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A55)){ //SC Control Point
		}else if(!uuid16_cmp(uuid , 0x2A07)){ //Tx Power Level
			char power = ((char*)value)[0];
			sprintf(buf, "Tx Power : %ddb", power);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A8A)){ //First Name
			strcpy(buf, "First Name : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A90)){ //Last Name
			strcpy(buf, "Last Name : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A87)){ //Email Address
			strcpy(buf, "Emall Address : ");
			strncat(buf, (char*)value, length);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A80)){ //Age
			strcpy(buf, "Age : ");
			sprintf(buf, "%s%d", buf, *(char*)value);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A85)){ //Date of Birth
			int year,mon,day;
			UINT8 *p = (UINT8*)value;
			STREAM_TO_UINT16(year, p);
			STREAM_TO_UINT8(mon, p);
			STREAM_TO_UINT8(day, p);
			if(year == 0 || mon == 0 || day == 0){
				strcpy(buf, "Date of birth is unknown");
			}else{
				sprintf(buf, "Date of birth : %d-%d-%d", year, mon, day);
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A8C)){ //Gender
			int gender = value[0];
			if(gender == 0){
				strcpy(buf, "Gender : Mail");
			}else if(gender == 1){
				strcpy(buf, "Gender : Femail");
			}else{
				strcpy(buf, "Gender Unspecified");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A98)){ //Weight
			int weight;
			UINT8 *p = (UINT8*)value;
			STREAM_TO_UINT16(weight, p);
			sprintf(buf, "Weight : %.2f kg", 0.005*(float)weight);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A8E)){ //Height
			int height;
			UINT8 *p = (UINT8*)value;
			STREAM_TO_UINT16(height, p);
			sprintf(buf, "Height : %.2f m", 0.01*(float)height);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A37)){ //Heart Rate Measurement
			char flag = value[0];
			if(flag & (1<<0)){ // 16bit bpm
				sprintf(buf, "Heart Rate Value : %d bpm", value[2]+(value[3]<<8));
			}else{ // 8bit bpm
				sprintf(buf, "Heart Rate Value : %d bpm", value[1]);
			}
			if(flag & (1<<3)){
				sprintf(buf, "Energy Expended : %d kJ", value[4]+(value[5]<<8));
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A38)){ //Body Sensor Location
			const char *location[] = {
				"Other", "Chest", "Wrist", "Finger", "Hand", "Ear Lobe", "Foot" };
			int key;
			if(length > 0){
				key = value[0];
				if(key < 0 || key >= sizeof(location)/sizeof(location[0])){
					key = 0;
				}
			}else{
				key = 0;
			}
			sprintf(buf, "Body Sensor Location : %s", location[key]);
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A39)){ //Heart Rate Control Point
			int ctrl = value[0];
			if(ctrl == 1){
				strcpy(buf, "Resets the value of the Energy Expended field in the Heart Rate");
				return buf;
			}
		}else if(!uuid16_cmp(uuid , 0x2A3F)){ //Alert Status
			int status = value[0];
			if(status & (1<<0)){
				strcpy(buf, "Ringer State active.");
			}else{
				strcpy(buf, "Ringer State not active.");
			}
			if(status & (1<<1)){
				strcat(buf, "  Vibrate State active.");
			}else{
				strcat(buf, "  Vibrate State not active.");
			}
			if(status & (1<<2)){
				strcat(buf, "  Display Alert Status State active.");
			}else{
				strcat(buf, "  Display Alert Status not active.");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A41)){ //Ringer Setting
			int setting = value[0];
			if(setting & (1<<0)){
				strcpy(buf, "Ringer Silent.");
			}else{
				strcpy(buf, "Ringer Normal.");
			}
			return buf;
		}else if(!uuid16_cmp(uuid , 0x2A40)){ //Ringer Control point
			int ctrl = value[0];
			if(ctrl == 1){
				strcpy(buf, "Silent Mode.");
			}else if(ctrl == 1){
				strcpy(buf, "Mute Once.");
			}else if(ctrl == 1){
				strcpy(buf, "Cancel Silent Mode.");
			}
			return buf;
		}
	}
	return NULL;
}
