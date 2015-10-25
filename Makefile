CFLAGS=-Wall -Wextra 

receiver: receiver.o read_write_loop.o create_socket.o real_address.o wait_for_client.o packet_implem.o -lm -lz

sender: sender.o read_write_loop.o create_socket.o real_address.o wait_for_client.o packet_implem.o -lm -lz
