#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void print_iter(DBusMessageIter *iter)
{
	dbus_bool_t valbool;
	dbus_uint32_t valu32;
	dbus_uint16_t valu16;
	dbus_int16_t vals16;
	unsigned char byte;
	const char *valstr;
	DBusMessageIter subiter;
	int type;
	if (iter == NULL) {
		printf("iter is nil\n");
		return;
	}
	while ((type=dbus_message_iter_get_arg_type(iter))!=DBUS_TYPE_INVALID){
		printf("(%c) ", type);
		switch (type) {
			case DBUS_TYPE_INVALID:
				printf("invalid\n");
				break;
			case DBUS_TYPE_STRING:
			case DBUS_TYPE_OBJECT_PATH:
				dbus_message_iter_get_basic(iter, &valstr);
				printf(" %s\n", valstr);
				break;
			case DBUS_TYPE_BOOLEAN:
				dbus_message_iter_get_basic(iter, &valbool);
				printf(" %s\n",
						valbool == TRUE ? "yes" : "no");
				break;
			case DBUS_TYPE_UINT32:
				dbus_message_iter_get_basic(iter, &valu32);
				printf(" 0x%06x\n", valu32);
				break;
			case DBUS_TYPE_UINT16:
				dbus_message_iter_get_basic(iter, &valu16);
				printf(" 0x%04x\n", valu16);
				break;
			case DBUS_TYPE_INT16:
				dbus_message_iter_get_basic(iter, &vals16);
				printf(" %d\n", vals16);
				break;
			case DBUS_TYPE_BYTE:
				dbus_message_iter_get_basic(iter, &byte);
				printf(" 0x%02x\n", byte);
				break;
			case DBUS_TYPE_VARIANT:
				dbus_message_iter_recurse(iter, &subiter);
				print_iter(&subiter);
				break;
			case DBUS_TYPE_ARRAY:
				dbus_message_iter_recurse(iter, &subiter);
				while (dbus_message_iter_get_arg_type(&subiter) !=
						DBUS_TYPE_INVALID) {
					print_iter(&subiter);
					dbus_message_iter_next(&subiter);
				}
				break;
			case DBUS_TYPE_DICT_ENTRY:
				dbus_message_iter_recurse(iter, &subiter);
				print_iter(&subiter);

				dbus_message_iter_next(&subiter);
				print_iter(&subiter);
				break;
			default:
				printf("unsupported type\n");
				break;
		}
		dbus_message_iter_next(iter);
	}
}
void receive()
{
	DBusMessage* msg;
	DBusMessageIter args;
	DBusConnection* conn;
	DBusError err;
	printf("Listening for signals\n");
	// initialise the errors
	dbus_error_init(&err);
	// connect to the bus and check for errors
	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set(&err)) {
		fprintf(stderr, "Connection Error (%s)\n", err.message);
		dbus_error_free(&err);
	}
	if (NULL == conn) {
		exit(1);
	}
	// add a rule for which messages we want to see
	dbus_bus_add_match(conn,
			"type='signal',interface='org.freedesktop.DBus.Properties'", &err);
	dbus_bus_add_match(conn,
			"type='signal',interface='org.freedesktop.DBus.ObjectManager'", &err);
	dbus_connection_flush(conn);
	if (dbus_error_is_set(&err)) {
		fprintf(stderr, "Match Error (%s)\n", err.message);
		exit(1);
	}
	while (true) {
		dbus_connection_read_write(conn, -1);
		msg = dbus_connection_pop_message(conn);
		// loop again if we haven't read a message
		if (NULL == msg) {
			continue;
		}
		printf("===== received msg - type: %s =====\n",
				dbus_message_type_to_string (dbus_message_get_type (msg)));
		printf("= destination : %s \n",
				dbus_message_get_destination (msg));
		printf("= interface : %s \n",
				dbus_message_get_interface(msg));
		printf("= path : %s \n",
				dbus_message_get_path(msg));
		printf("= member : %s \n",
				dbus_message_get_member(msg));
		dbus_message_iter_init(msg, &args);
		print_iter(&args);
		printf("\n");
		// free the message
		dbus_message_unref(msg);
	}
}
int main(int argc, char** argv)
{
	receive();
	return 0;
}
