OBJS=dht22.o send_recv_msg.o
CFLAGS=-Wall
LIB=libclient.a
CC=gcc
AR=ar
INC=-I../include

libclient.a: $(OBJS)
	$(AR) rv $(LIB) $(OBJS)
%.o: %.c
	$(CC) -c $(INC) $(CFALGS) $< -o $@
clean:
	rm -f *.o $(LIB) $(OBJS)
