CC ?= gcc
CFLAGS := -Wall

all : server client

BIN_DIR := bin/
bindir :
	@if test ! -d $(BIN_DIR) ; \
    	then \
        	mkdir $(BIN_DIR) ; \
    fi

SERVER_DIR := src/server/

server : server_main.o socket_server.o bindir
	$(CC) $(CFLAGS) -o bin/server $(SERVER_DIR)main.o $(SERVER_DIR)socket_server.o

server_main.o : socket_server.o
	cd $(SERVER_DIR) && $(CC) $(CFLAGS) -c -o main.o main.c
socket_server.o : 
	cd $(SERVER_DIR) && $(CC) $(CFLAGS) -c -o socket_server.o socket_server.c


CLIENT_DIR = src/client/

client : client_main.o socket_client.o bindir
	$(CC) -o bin/client $(CLIENT_DIR)main.o $(CLIENT_DIR)socket_client.o

socket_client.o : 
	cd $(CLIENT_DIR) && $(CC) $(CFLAGS) -c -o socket_client.o socket_client.c
client_main.o : socket_client.o
	cd $(CLIENT_DIR) && $(CC) $(CFLAGS) -c -o main.o main.c

SRC_DIR = src/

skb_test : skb_test.o sk_buff.o
	cd $(SRC_DIR) && $(CC) -o skb_test skb_test.o sk_buff.o

skb_test.o : 
	cd $(SRC_DIR) && $(CC) $(CFLAGS) -c -o skb_test.o sk_buff_test.c
sk_buff.o :
	cd $(SRC_DIR) && $(CC) $(CFLAGS) -c -o sk_buff.o sk_buff.c

.PHONY : clean
clean: 
	rm src/server/*.o src/client/*.o src/*.o
