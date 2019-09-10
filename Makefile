all:
	gcc macchanger.c -o macchanger `pkg-config --cflags --libs gtk+-3.0` -w
