/*
 * avrcp_ctrl.h
 *
 * Copyright (C) 2017 Ingenic Semiconductor Co., Ltd
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

#ifndef AVRCP_CTRL_H__
#define AVRCP_CTRL_H__

#define PATH_DBUS_INGENIC_AVRCP_CTRL   "/ingenic/avrcp/ctrl"
#define PATH_DBUS_INGENIC_AVRCP_STATUS "/ingenic/avrcp/status"
#define INTERFACE_FREEDESKTOP_DBUS     "org.freedesktop.DBus.ObjectManager"

dbus_bool_t init_avrcp_srv(DBusConnection* conn);
dbus_bool_t process_avrcp_status(DBusConnection *conn, DBusMessage *msg);
dbus_bool_t process_avrcp_ctrl(DBusConnection *conn, DBusMessage *msg);

#endif /* AVRCP_CTRL_H__ */
