CFLAGS=-Wall  -Wextra

receiver: receiver.o read_write_loop.o create_socket.o real_address.o wait_for_client.o
