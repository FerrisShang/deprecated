extern struct bluetooth_plugin_desc __bluetooth_builtin_hostname;
extern struct bluetooth_plugin_desc __bluetooth_builtin_wiimote;
extern struct bluetooth_plugin_desc __bluetooth_builtin_autopair;
extern struct bluetooth_plugin_desc __bluetooth_builtin_policy;
extern struct bluetooth_plugin_desc __bluetooth_builtin_neard;
extern struct bluetooth_plugin_desc __bluetooth_builtin_sap;
extern struct bluetooth_plugin_desc __bluetooth_builtin_a2dp;
extern struct bluetooth_plugin_desc __bluetooth_builtin_avrcp;
extern struct bluetooth_plugin_desc __bluetooth_builtin_network;
extern struct bluetooth_plugin_desc __bluetooth_builtin_input;
extern struct bluetooth_plugin_desc __bluetooth_builtin_hog;
extern struct bluetooth_plugin_desc __bluetooth_builtin_health;
extern struct bluetooth_plugin_desc __bluetooth_builtin_gap;
extern struct bluetooth_plugin_desc __bluetooth_builtin_scanparam;
extern struct bluetooth_plugin_desc __bluetooth_builtin_deviceinfo;
extern struct bluetooth_plugin_desc __bluetooth_builtin_alert;
extern struct bluetooth_plugin_desc __bluetooth_builtin_time;
extern struct bluetooth_plugin_desc __bluetooth_builtin_proximity;
extern struct bluetooth_plugin_desc __bluetooth_builtin_thermometer;
extern struct bluetooth_plugin_desc __bluetooth_builtin_heartrate;
extern struct bluetooth_plugin_desc __bluetooth_builtin_cyclingspeed;
extern struct bluetooth_plugin_desc __bluetooth_builtin_gatt_example;

static struct bluetooth_plugin_desc *__bluetooth_builtin[] = {
  &__bluetooth_builtin_gatt_example,
  //&__bluetooth_builtin_network,
  //&__bluetooth_builtin_input,
  &__bluetooth_builtin_heartrate,
  //&__bluetooth_builtin_deviceinfo,
  &__bluetooth_builtin_autopair,
  //&__bluetooth_builtin_hostname,
  //&__bluetooth_builtin_neard,
  //&__bluetooth_builtin_policy,
  //&__bluetooth_builtin_wiimote,
  NULL
};
