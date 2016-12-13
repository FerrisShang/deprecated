1. 系统启动后先启动蓝牙

/ # bt_enable
=========read from uart_fd===(null)=Done setting line discpline
Broadcom firmware initialized.

2. 蓝牙启动后先查看系统后台是否有bluetoothd进程在运行, 由于bluetoothd可能会抢占btgatt-client的ATT监听,因此最好先kill掉bluetoothd进程.

3. 扫描需要连接的设备
/ # hcitool lescan
LE Scan ...
50:31:36:3A:90:66 D0:31:36:BB:3B:19
50:31:36:3A:90:66 (unknown)
D2:D2:88:32:CC:E9 MIO GLOBAL
D2:D2:88:32:CC:E9 (unknown)
46:2B:67:BB:C3:8A MPEN46:2B:67:BB:C3:8A
46:2B:67:BB:C3:8A (unknown)

4. 运行 btgatt-client, 参数如下:
btgatt-client
Usage:
	btgatt-client [options]
Options:
	-i, --index <id>		Specify adapter index, e.g. hci0
	-d, --dest <addr>		Specify the destination address
	-t, --type [random|public] 	Specify the LE address type
	-m, --mtu <mtu> 		The ATT MTU to use
	-s, --security-level <sec> 	Set security level (low|medium|high)
	-v, --verbose			Enable extra logging
	-h, --help

比如我们要连接上面搜索到的 MIO手表(D2:D2:88:32:CC:E9 MIO GLOBAL)
则指令为:
btgatt-client -i hci0 -d D2:D2:88:32:CC:E9 -t random -s low

5. 查看设备支持的服务
连接后程序自动搜索该设备的services 和 characterisc 等并打印.
a.对于程序可解析的service 和 characterisc 程序会直接打印出其含义, 并在其后注明对应的 UUID, 如:
service - Device Information(0x180A)
	  charac - handle: 0x0010 , Serial Number String(0x2A25)
		  Permission - Read
代表services 为Device Information UUID 为 0x180A, 其中包括一个 "Serial Number String" 的characterisc, 对应的handle 为0x0010,权限为Read
b.对于程序不能解析的services 和 characterisc 程序只打印出UUID, 如:
service - start: 0x0009, end: 0x000d, type: primary, uuid: 6c721826-5bf1-4f64-9170-381c08ec57ee
	  charac - start: 0x000a, value: 0x000b, props: 0x0a, uuid: 6c722a0a-5bf1-4f64-9170-381c08ec57ee
代表services的uuid为6c721826-5bf1-4f64-9170-381c08ec57ee, 整个service 占据的handle范围是 0x0009~0x000d, 其中包含一个UUID为6c722a0a-5bf1-4f64-9170-381c08ec57ee的characterisc

6. 执行读写操作
在程序的提示符下直接按回车, 会有支持指令的提示:
[GATT client]#
Commands:
	help           		Display help message
	services       		Show discovered services
	read-value     		Read a characteristic or descriptor value
	write-value    		Write a characteristic or descriptor value
	register-notify		Subscribe to not/ind from a characteristic
	unregister-notify	Unregister a not/ind session
read-value对应Read权限
write-value对应Write权限
register-notify对应Notify权限

a.read-value 用法
Usage: read-value <value_handle>
比如:
[GATT client]# read-value 0x0059
[GATT client]#
Read value [Body Sensor Location(0x2A38)]
Body Sensor Location : Other


b.write-value 用法
Usage: write-value [options] <value_handle> <value>
Options:
	-w, --without-response	Write without response
	-s, --signed-write	Signed write command
e.g.:
	write-value 0x0001 00 01 00
比如:
[GATT client]# write-value -w 0x0056 1
Resets the value of the Energy Expended field in the Heart Rate
Write command sent
(注意:<value>值为十进制)

c.register-notify 用法
Usage: register-notify <chrc value handle>
比如:
[GATT client]# register-notify 0x005b
Registering notify handler with id: 2
[GATT client]#
Registered notify handler!
[GATT client]#
	Handle Value Not/Ind: Heart Rate Measurement(0x2A37)
	Heart Rate Value : 100 bpm
[GATT client]#
	Handle Value Not/Ind: Heart Rate Measurement(0x2A37)
	Heart Rate Value : 80 bpm
注册notify 成功后当server有消息发出时, 程序会自动解析指令并打印

d.unregister-notify 用法
程序上暂时不支持unregister-notify指令

7. Service及Characterisc定义
UUID 相关定义和service中characterisc 值的含义可参考官方网站 https://www.bluetooth.com/specifications/gatt
