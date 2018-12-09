CC=gcc
LINK=-lclient -lpthread -lwiringPi
OBJS=client.o
CFILE=client.c

client: $(OBJS)
	$(CC) -o ../client -L../lib $(OBJS) $(LINK)
$(OBJS): $(CFILE)
	$(CC) -c -I../include $(CFILE)
clean:
	rm -f $(OBJS) ../client
