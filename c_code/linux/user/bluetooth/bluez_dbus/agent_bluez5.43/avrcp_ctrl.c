#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "avrcp_ctrl.h"

static char player_path[80];
static char fd_path[80];

dbus_bool_t init_avrcp_srv(DBusConnection* conn)
{
	DBusError err;
	// initialise the errors
	dbus_error_init(&err);
	// add a rule for which messages we want to see
	dbus_bus_add_match(conn,
			"type='signal',"
			"interface='org.freedesktop.DBus.ObjectManager'",
			&err);
	dbus_connection_flush(conn);
	if (dbus_error_is_set(&err)) {
		fprintf(stderr, "Match Error (%s)\n", err.message);
		return FALSE;
	}
	return TRUE;
}
dbus_bool_t process_avrcp_status(DBusConnection *conn, DBusMessage *msg)
{
	const char *member;
	DBusMessageIter args;
	char *path;
	member = dbus_message_get_member(msg);
	if(member && !strcmp(member, "InterfacesAdded")){
		dbus_message_iter_init(msg, &args);
		dbus_message_iter_get_basic(&args, &path);
		if(strstr(path, "/player")){
			strcpy(player_path, path);
			printf("player_path : %s\n", path);
		}else if(strstr(path, "/fd")){
			strcpy(fd_path, path);
			printf("fd_path : %s\n", path);
		}
		return TRUE;
	}else if(member && !strcmp(member, "InterfacesRemoved")){
		dbus_message_iter_init(msg, &args);
		dbus_message_iter_get_basic(&args, &path);
		if(strstr(path, "/player")){
			printf("player_path removed\n");
			player_path[0] = '\0'; 
		}else if(strstr(path, "/fd")){
			fd_path[0] = '\0'; 
			printf("fd_path removed\n");
		}
		return TRUE;
	}
	return FALSE;
}

static void media_ctrl(DBusConnection* conn, char *path, char *method)
{
#define DEST_BLUEZ    "org.bluez"
#define INTF_MEDIA    "org.bluez.MediaPlayer1"
	DBusMessage* msg;
	if(path[0] == '\0'){
		return;
	}
	msg = dbus_message_new_method_call(
			DEST_BLUEZ, path, INTF_MEDIA, method);
	dbus_connection_send(conn, msg, NULL);
	dbus_message_unref(msg);
}
#define PATH_DBUS_INGENIC_AVRCP_CTRL "/ingenic/avrcp/ctrl"
dbus_bool_t process_avrcp_ctrl(DBusConnection *conn, DBusMessage *msg)
{
	const char *member, *path;
	path = dbus_message_get_path(msg);
	if(path && !strcmp(path, PATH_DBUS_INGENIC_AVRCP_CTRL)){
		member = dbus_message_get_member(msg);
		if(member != NULL){
			if(!strcmp(member, "Play")){
				media_ctrl(conn, player_path, "Play");
				return TRUE;
			}else if(!strcmp(member, "Stop")){
				media_ctrl(conn, player_path, "Stop");
				return TRUE;
			}else if(!strcmp(member, "Pause")){
				media_ctrl(conn, player_path, "Pause");
				return TRUE;
			}else if(!strcmp(member, "Next")){
				media_ctrl(conn, player_path, "Next");
				return TRUE;
			}else if(!strcmp(member, "Previous")){
				media_ctrl(conn, player_path, "Previous");
				return TRUE;
			}
		}
	}
	return FALSE;
}
