all: libgsttruehddec appsrc-appsink filesrc-appsink


libgsttruehddec:
	gcc -Wall -fPIC -pthread -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -c -o gsttruehddec.o gsttruehddec.c 
	gcc -shared -o libgsttruehddec.so gsttruehddec.o -pthread -L/usr/local/lib -lgstreamer-1.0 -lgobject-2.0 -lgthread-2.0 -lrt -lglib-2.0 -L. -lgstaudio-1.0

appsrc-appsink:
	gcc appsrc-appsink.c -o aa `pkg-config --cflags --libs gstreamer-1.0` -lgstapp-1.0

filesrc-appsink:
	gcc filesrc-appsink.c -o fa `pkg-config --cflags --libs gstreamer-1.0` -lgstapp-1.0

clean:
	rm *.o *.so aa fa

install:
