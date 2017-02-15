#ifndef PAIR_CTRL_H__
#define PAIR_CTRL_H__

dbus_bool_t init_pairable(DBusConnection* conn);
dbus_bool_t process_pair_req(DBusConnection *conn, DBusMessage *msg);
dbus_bool_t process_service_req(DBusConnection *conn, DBusMessage *msg);

#endif /* PAIR_CTRL_H__ */
