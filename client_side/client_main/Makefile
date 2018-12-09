CC=gcc
EXE=/home/pi/ESW_Final_Project/client_side/client
LINK=-lclient -lpthread -lwiringPi
OBJS=client.o
CFILE=client.c

$(CFILE): $(OBJS)
	$(CC) -o $(EXE) -L../lib $(OBJS) $(LINK)
$(OBJS): $(CFILE)
	$(CC) -c -I../include $(CFILE)
clean:
	rm -f $(OBJS) $(EXE)
