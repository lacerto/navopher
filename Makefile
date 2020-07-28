CFLAGS  = $(shell pkg-config --cflags glib-2.0 gio-2.0)
LDLIBS  = $(shell pkg-config --libs glib-2.0 gio-2.0)
OBJECTS = navopher.o gmap.o utils.o
HEADERS = gmap.h utils.h

navopher: $(OBJECTS) $(HEADERS)
		$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

.PHONY: test clean
test:
		./test.sh

clean:
		rm -rf testdir
		rm -f $(OBJECTS)
		rm -f navopher
