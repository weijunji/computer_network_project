CC := gcc

socket : main.o socket_client.o
	$(CC) -o main main.o socket_client.o

socket_client.o : 
main.o : 

.PHONY : clean
clean:
	rm *.o
