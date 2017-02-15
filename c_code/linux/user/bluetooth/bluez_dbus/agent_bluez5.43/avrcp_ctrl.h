#ifndef AVRCP_CTRL_H__
#define AVRCP_CTRL_H__

dbus_bool_t init_avrcp_srv(DBusConnection* conn);
dbus_bool_t process_avrcp_status(DBusConnection *conn, DBusMessage *msg);
dbus_bool_t process_avrcp_ctrl(DBusConnection *conn, DBusMessage *msg);

#endif /* AVRCP_CTRL_H__ */
