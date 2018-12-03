client: dht22.o send_recv_msg.o libclient.a
	gcc -o client client.c -L. -lclient -lwiringPi -lpthread
libclient.a: dht22.o send_recv_msg.o
	ar rv libclient.a dht22.o send_recv_msg.o
%.o: %.c
	gcc -c $< -o $@
clean:
	rm -f *.o libclient client
