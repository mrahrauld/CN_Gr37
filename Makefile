CC			= gcc
FLAGS 		= -Wall -g
LIBS		= -lz  -lm 

PROG		= sender receiver tests
LISTEOBJ 	= packet.o real_address.o create_socket.o wait_for_client.o

all					: $(PROG)

sender				: $(LISTEOBJ) sender.o
	$(CC) -o sender $(LISTEOBJ) sender.o $(FLAGS) $(LIBS)
	./sender -f m3.pdf ::1 64536

receiver			: $(LISTEOBJ) receiver.o
	$(CC) -o receiver $(LISTEOBJ) receiver.o $(FLAGS) $(LIBS)
	./receiver -f coucou.pdf ::1 64536
tests				: $(LISTEOBJ) tests.o
	$(CC) -o tests $(LISTEOBJ) tests.o $(FLAGS) $(LIBS)

sender.o			: sender.c
	$(CC) -c sender.c -o sender.o $(FLAGS)

receiver.o			: receiver.c
	$(CC) -c receiver.c -o receiver.o $(FLAGS)

packet.o			: packet_implem.c
	$(CC) -c packet_implem.c -o packet.o $(FLAGS) $(LIBS)

real_address.o		: real_address.c
	$(CC) -c real_address.c -o real_address.o $(FLAGS)

create_socket.o		: create_socket.c
	$(CC) -c create_socket.c -o create_socket.o $(FLAGS)

wait_for_client.o	: wait_for_client.c
	$(CC) -c wait_for_client.c -o wait_for_client.o $(FLAGS)
tests.o			:test.c
	$(CC) -c tests.c -o tests.o $(FLAGS)	

clean				:
	rm -vf $(LISTEOBJ)

mrproper			:
	rm -vf $(LISTEOBJ) $(PROG)
