#include "socket_server.h"

void _cleanWSA() {
#ifdef WIN32
	WSACleanup();
#endif // WIN32
}

void init_socket(int* sockfd, int port) {
	struct sockaddr_in ServerAddr;

	// init WSA
	#ifdef WIN32
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(1, 1), &WSAData)){
		printf("ERROR: setup WSA\n");
		_cleanWSA();
		exit(-1);
	}
	#endif

	// init socket
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0){
		printf("ERROR: create socket\n");
		_cleanWSA();
		exit(-1);
	}

	// set server
	ServerAddr.sin_family = AF_INET; // ipv4
	ServerAddr.sin_port = htons(port); // port
	ServerAddr.sin_addr.s_addr = INADDR_ANY; // local host
	memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

	// set socket
	if (bind(*sockfd, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) == -1){
		printf("ERROR: bind\n");
		_cleanWSA();
		exit(-1);
	}

	// listen
	if (listen(*sockfd, BACKLOG) == -1) {
		printf("ERROR: listen\n");
		_cleanWSA();
		exit(-1);
	}
}

int recieve(int* sockfd, char* buf, int size) {
	int sin_size;
	struct sockaddr_in clientAddr;
	int client_sock;

	sin_size = sizeof(struct sockaddr_in);
	// wait for client
	client_sock = accept(*sockfd, (struct sockaddr *)&clientAddr, &sin_size);

	if (client_sock == -1) {
		printf("ERROR: accept\n");
		_cleanWSA();
		exit(-1);
	}

	printf("Connect to %s\n", inet_ntoa(clientAddr.sin_addr));

	int numbytes = recv(client_sock, buf, size, 0);

	if (numbytes == -1)
	{
		printf("ERROR: recv");
		_cleanWSA();
		exit(-1);
	}

	buf[numbytes] = '\0';

	// printf("Received: %s\n", buf);
	return numbytes;
}
