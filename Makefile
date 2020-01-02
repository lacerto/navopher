navopher: navopher.c
		$(CC) `pkg-config --cflags glib-2.0` $< -o $@ `pkg-config --libs glib-2.0`
