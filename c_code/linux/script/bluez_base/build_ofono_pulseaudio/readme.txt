pulseaudio-6.0
ofono-1.16
bluez-4.101
glib-2.40.0
zlib-1.2.8
dbus-1.8.8
expat-2.1.0
libsndfile-1.0.25
libffi-3.1
libtool-2.4
json-c-0.12



	 bluez-4.101
		  glib-2.40.0
			   zlib-1.2.8
			   libffi-3.1
		  dbus-1.8.8
			   expat-2.1.0
	 ofono-1.16
		  glib-2.40.0
		  dbus-1.8.8

	 pulseaudio-6.0
		  libtool-2.4
		  libsndfile-1.0.25
		  json-c-0.12
		  dbus-1.8.8
		  bluez-4.101

export PREFIX=***/output
export HOST=mips-linux-gnu
export CC=mips-linux-gnu-gcc

expat-2.1.0/config_expat-2.1.0.sh
dbus-1.8.8/config_dbus-1.8.8.sh
zlib-1.2.8/config_zlib-1.2.8.sh
libffi-3.1/config_libffi-3.1.sh
glib-2.40.0/config_glib-2.40.0.sh
bluez-4.101/config_bluez-4.101.sh
ofono-1.16/config_ofono-1.16.sh
libtool-2.4/config_libtool-2.4.sh
libsndfile-1.0.25/config_libsndfile-1.0.25.sh
json-c-0.12/config_json-c-0.12.sh
sbc-1.3/config_sbc-1.3.sh
pulseaudio-6.0/config_pulseaudio-6.0.sh
