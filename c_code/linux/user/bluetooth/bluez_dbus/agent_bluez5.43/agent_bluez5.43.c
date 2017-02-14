/*
 * agent.c  - Auto process pairing request
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

#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define DEST_BLUEZ    "org.bluez"
#define PATH_BLUEZ    "/org/bluez"
#define PATH_AGENT    "/org/bluez/agent"
#define INTF_AGENT_MG "org.bluez.AgentManager1"
#define INTF_AGENT    "org.bluez.Agent1"
#define CAPABILITY    "NoInputNoOutput"

char *path2mac(char *path)
{
	static char mac[] = "--:--:--:--:--:--";
	char *p, *mac_pre = "dev_";
	int i;
	p = strstr(path, mac_pre);
	for(i=0;i<6;i++){
		mac[i*3+0] = *(p+i*3+strlen(mac_pre)+0);
		mac[i*3+1] = *(p+i*3+strlen(mac_pre)+1);
	}
	return mac;
}
int main(int argc, char** argv)
{
   DBusMessage* msg;
   DBusMessageIter args;
   DBusConnection* conn;
   DBusPendingCall* pending;

   // connect to the system bus and check for errors
   conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
   if (NULL == conn) {
      exit(1);
   }

   //Register agent
   msg = dbus_message_new_method_call(
		   DEST_BLUEZ, PATH_BLUEZ, INTF_AGENT_MG, "RegisterAgent");
   dbus_message_iter_init_append(msg, &args);
   const char *path_reg = PATH_AGENT;
   const char *capability = CAPABILITY;
   dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &path_reg);
   dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &capability);
   if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)){
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }
   dbus_connection_flush(conn);
   dbus_message_unref(msg);
   dbus_pending_call_block(pending);
   msg = dbus_pending_call_steal_reply(pending);
   dbus_pending_call_unref(pending);
   dbus_message_unref(msg);

   //Request default agent
   msg = dbus_message_new_method_call(
		   DEST_BLUEZ, PATH_BLUEZ, INTF_AGENT_MG, "RequestDefaultAgent");
   dbus_message_iter_init_append(msg, &args);
   const char *path_req = PATH_AGENT;
   dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &path_req);
   if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)){
	   fprintf(stderr, "Out Of Memory!\n");
	   exit(1);
   }
   dbus_connection_flush(conn);
   dbus_message_unref(msg);
   dbus_pending_call_block(pending);
   msg = dbus_pending_call_steal_reply(pending);
   dbus_pending_call_unref(pending);
   dbus_message_unref(msg);

   //Wait for processing pair request
	while (true) {
		//blocking read of the next available message
		dbus_connection_read_write(conn, 500);
		msg = dbus_connection_pop_message(conn);
		if(msg != NULL){
			if(dbus_message_is_method_call(msg, INTF_AGENT, "RequestAuthorization")){
				DBusMessage* reply;
				char *path;
				dbus_message_get_args(msg, NULL,
						DBUS_TYPE_OBJECT_PATH, &path,
						DBUS_TYPE_INVALID);
				reply = dbus_message_new_method_return(msg);
				dbus_connection_send(conn, reply, NULL);
				printf("Pairing with device : %s\n", path2mac(path));
			}
			if(dbus_message_is_method_call(msg, INTF_AGENT, "AuthorizeService")){
				DBusMessage* reply;
				char *path, *uuid;
				dbus_message_get_args(msg, NULL,
						DBUS_TYPE_OBJECT_PATH, &path,
						DBUS_TYPE_STRING, &uuid,
						DBUS_TYPE_INVALID);
				reply = dbus_message_new_method_return(msg);
				dbus_connection_send(conn, reply, NULL);
				printf("AuthorizeService - UUID:%s\n", uuid);
			}
		}
	}
}
