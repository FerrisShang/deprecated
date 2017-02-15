#ifndef AVRCP_CTRL_H__
#define AVRCP_CTRL_H__

#define PATH_DBUS_INGENIC_AVRCP_CTRL   "/ingenic/avrcp/ctrl"
#define PATH_DBUS_INGENIC_AVRCP_STATUS "/ingenic/avrcp/status"
#define INTERFACE_FREEDESKTOP_DBUS     "org.freedesktop.DBus.ObjectManager"

dbus_bool_t init_avrcp_srv(DBusConnection* conn);
dbus_bool_t process_avrcp_status(DBusConnection *conn, DBusMessage *msg);
dbus_bool_t process_avrcp_ctrl(DBusConnection *conn, DBusMessage *msg);

#endif /* AVRCP_CTRL_H__ */
