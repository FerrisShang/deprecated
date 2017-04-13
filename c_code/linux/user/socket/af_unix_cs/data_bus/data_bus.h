#ifndef DATA_BUS_H__
#define DATA_BUS_H__

/* command of data bus */
enum {
	CMD_CONNECTED = 0,
	CMD_DISCONNECTED,
	CMD_SEND_DATA,
};

/* type of devices */
enum {
	DEV_TYPE_LIGHT = 0,
	DEV_TYPE_FAN,
	DEV_TYPE_MIO_WATCH,
	DEV_TYPE_SZ_WATCH,
	DEV_TYPE_SCALE,
	DEV_TYPE_QRCODE,
	DEV_TYPE_MAX,
	DEV_TYPE_INVALID = 0xFF
};

struct light {
	int cmd;
	int status;
};
struct fan {
	int cmd;
	int status;
};
struct mio_watch {
	int cmd;
	int heart_rate;
};
struct sz_watch {
	int cmd;
	int heart_rate;
};
struct scale {
	int cmd;
	float weight;
};
struct qr_code{
	int cmd;
	char string[512];
};

struct bus_server;
struct bus_client;
struct bus_data {
	int dev_type;
	int dev_id;
	union {
		struct light light;
		struct fan fan;
		struct mio_watch mio_watch;
		struct sz_watch sz_watch;
		struct scale scale;
		struct qr_code qr_code;
	};
};

/** Callback of receicing data (both of server & client)
 *
 * @param data : data received
 * @param p : point of user data
 */
typedef int (*data_bus_cb)(struct bus_data *data, void *p);

/** Create data bus server
 *
 * @param callback : Callback when receicing data from client
 * @param p : point of user data
 * Note : Server cannot be destroyed
 */
struct bus_server* bus_server_create(data_bus_cb callback, void *p);

/** Create data bus client
 *
 * @param dev_type : Type of the device
 * @param dev_id : ID of the device
 * @param cb : Callback when receicing data from server
 * @param p : point of user data
 * Note : Client cannot be destroyed
 * Warning : A pair of dev_type and dev_id can only create one client
 */
struct bus_client* bus_client_create(int dev_type, int dev_id,
		data_bus_cb cb, void *p);

/** Send data from server to client
 *
 * @param srv : Point of server created by bus_server_create
 * @param data : data to client, client dev_type & dev_id must be filled in data
 */
int bus_server_send(struct bus_server* srv, struct bus_data *data);

/** Send data from client to server
 *
 * @param cli : Point of client created by bus_client_create 
 * @param data : data to server, client dev_type & dev_id in data can be omitted
 */
int bus_client_send(struct bus_client* cli, struct bus_data *data);

#endif /* DATA_BUS_H__ */
