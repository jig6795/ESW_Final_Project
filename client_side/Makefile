OBJS=dht22.o send_recv_msg.o
LIB=libclient.a
LINK=-lclient -lwiringPi -lpthread
CC=gcc
AR=ar
RES=client

$(RES): $(OBJS) $(LIB)
	$(CC) -o $(RES) client.c -L. $(LINK)
libclient.a: $(OBJS)
	$(AR) rv $(LIB) $(OBJS)
%.o: %.c
	$(CC) -c $< -o $@
clean:
	rm -f *.o libclient $(RES)
