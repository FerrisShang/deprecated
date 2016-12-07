#ifndef BLE_PROFILE_H__
#define BLE_PROFILE_H__

#define UUID_SERV_AUTOMATION_IO        "1815"
#define UUID_CHAC_DIGITAL              "2A56" //permission READ, WRITE
#define CHAC_DIGITAL_INACTIVE          0x00
#define CHAC_DIGITAL_ACTIVE            0x01
#define CHAC_DIGITAL_TRI_STATE         0x02
#define CHAC_DIGITAL_OUTPUT_STATE      0x03
#define UUID_CHAC_ANALOG               "2A58" //permission READ, WRITE
//data uint16
#define UUID_CHAC_AGGREGATE            "2A5A" //permission READ, WRITE
//data : Digital+Analog

#define UUID_SERV_BATTERY              "180F"
#define UUID_CHAC_BATTERY_LEVEL        "2A19" //permission READ, NOTIFY
//data : 0 ~ 100

#define UUID_SERV_CURRENT_TIME         "1805"
#define UUID_CHAC_CURRENT_TIME         "2A2B" //permission READ, NOTIFY
//data : (u16+u8+u8+u8+u8+u8+u8+Fractions256) - yy+mm+dd+hh+mm+ss+week+F, 0 means unknown

#define UUID_SERV_DEVICE_INFORMATION   "180A"
#define UUID_CHAC_MODEL_NUMBER         "2A24" //permission READ, uint8s
#define UUID_CHAC_SERIAL_NUMBER        "2A25" //permission READ, uint8s
#define UUID_CHAC_HARDWARE_REVISION    "2A27" //permission READ, uint8s
#define UUID_CHAC_FIRMWARE_REVISION    "2A26" //permission READ, uint8s
#define UUID_CHAC_SOFTWARE_REVISION    "2A28" //permission READ, uint8s
#define UUID_CHAC_SYSTEM_ID            "2A23" //permission READ, uint40+uint24
#define UUID_CHAC_RC_DATA_LIST         "2A2A" //permission READ
#define UUID_CHAC_PNP_ID               "2A50" //permission READ

#define UUID_SERV_GENERIC_ACCESS       "1800"
#define UUID_CHAC_DEVICE_NAME          "2A00" //permission READ, uint8s
#define UUID_CHAC_APPEARANCE           "2A01" //permission READ, 16bits
/* Enumerations
		  0     Unknown
		  64    Generic Phone
		  128   Generic Computer
		  192   Generic Watch
		  193   Watch: Sports
		  256   Generic Clock
		  320   Generic Display
		  384   Generic Remote Control
		  448   Generic Eye-glasses
		  512   Generic Tag
		  576   Generic Keyring
		  640   Generic Media Player
		  704   Generic Barcode Scanner
		  768   Generic Thermometer
		  769   Thermometer: EarThermometer subtype
		  832   Generic Heart rate Sensor
		  833   Heart Rate Sensor: Heart Rate Belt
		  896   Generic Blood Pressure
		  897   Blood Pressure: Arm
		  898   Blood Pressure: Wrist
		  960   Human Interface Device (HID)
		  961   Keyboard
		  962   Mouse
		  963   Joystick
		  964   Gamepad
		  965   Digitizer Tablet
		  966   Card Reader
		  967   Digital Pen
		  968   Barcode ScannerGeneric
		  1024  Generic Glucose Meter
		  1088  Generic: Running Walking Sensor
		  1089  Running Walking Sensor: In-ShoeRunning
		  1090  Running Walking Sensor: On-ShoeRunningRunning
		  1091  Running Walking Sensor: On-HipRunning
		  1152  Generic: Cycling
		  1153  Cycling: Cycling Computer
		  1154  Cycling: Speed Sensor
		  1155  Cycling: Cadence Sensor
		  1156  Cycling: Power Sensor
		  1157  Cycling: Speed and Cadence Sensor
		  3136  Generic: Pulse Oximeter
		  3137  Fingertip
		  3138  Wrist Worn
		  3200  Generic: Weight Scale
		  5184  Generic: Outdoor Sports Activity
		  5185  Location Display Device
		  5186  Location and Navigation Display Device
		  5187  Location Pod
		  5188  Location and Navigation Pod
*/
#define UUID_CHAC_PERIPHERAL_PRIVACY   "2A02" //permission READ, boolean
#define UUID_CHAC_RECONNECTION_ADDRESS "2A03" //permission READ, uint48
#define UUID_CHAC_CONNECTION_PARAM     "2A04" //permission READ
//data : (u16+u16+u16+u16) - minInterval+maxInterval+latency+Timeout

#define UUID_SERV_HEART_RATE           "180d"
#define UUID_CHAC_HR_MEASUREMENT       "2A37" //permission READ, NOTIFY
//data : (u8+u8+...) - flag + value
#define UUID_CHAC_BODY_SENSOR_LOCATION "2A38" //permission READ
//data : 0-Other 1-Chest 2-Wrist 3-Finger 4-Hand 5-EarLobe 6-Foot

#define UUID_SERV_IMMEDIATE_ALERT      "1802"
#define UUID_CHAC_ALERT_LEVEL          "2A06" //permission WriteWithoutResponse
//data : 0-No Alert 1-Mild Alert 2-High Alert

#define UUID_SERV_RUNNING_SPEED        "1814"
#define UUID_CHAC_RSC_MEASUREMENT      "2A53" //permission NOTIFY
//data : (u8+u16+u8) - flag+Speed+Cadence

#define UUID_SERV_TX_POWER             "1804"
#define UUID_CHAC_TX_POWER_LEVEL       "2A07" //permission READ (-100~20)

#define UUID_SERV_USER_DATA            "181C"
#define UUID_CHAC_FIRST_NAME           "2A8A" //permission READ, WRITE
#define UUID_CHAC_LAST_NAME            "2A90" //permission READ, WRITE
#define UUID_CHAC_EMAIL_ADDRESS        "2A87" //permission READ, WRITE
#define UUID_CHAC_AGE                  "2A80" //permission READ, WRITE (u8)

#define UUID_SERV_PHONE_ALERT_STATUS   "180E"
#define UUID_CHAC_ALERT_STATUS         "2A3F" //permission READ, NOTIFY
/* Enumerations
	 Bit 0, meaning “Ringer State”
	 Bit 1, meaning “Vibrator State”
	 Bit 2, meaning "Display Alert Status"
*/
#define UUID_CHAC_RINGER_SETTING       "2A41" //permission READ, NOTIFY
//data : 0-Ringer Silent 1-Ringer Normal
#define UUID_CHAC_RINGER_CONTROL_POINT "2A40" //permission WriteWithoutResponse
//data : 1-Silent Mode 2-Mute Once 3-Cancel Silent Mode

#endif /* BLE_PROFILE_H__ */
