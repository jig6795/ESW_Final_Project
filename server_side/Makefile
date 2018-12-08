
CC = gcc
LIBOBJ = send_total.o handle_clnt.o
TLIB = libfunc.a
LIBPATH = ./library
server_running: $(TLIB) server.c
	$(CC) -o server_running server.c -L. -lfunc

$(TLIB): $(LIBOBJ)
	ar rv $(TLIB) $(LIBOBJ)

send_total.o: 
	$(CC) -c $(LIBPATH)/send_total.c

handle_clnt.o: 
	$(CC) -c $(LIBPATH)/handle_clnt.c

clean:
	rm $(LIBOBJ) $(TLIB) server_running