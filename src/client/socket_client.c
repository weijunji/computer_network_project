#include "socket_client.h"

void _cleanWSA() {
#ifdef WIN32
	WSACleanup();
#endif
}

void init_socket(int* sockfd, char* ip, int port) {
	
#ifdef WIN32
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(1, 1), &WSAData))
	{
		printf("ERROR: init WSA\n");
		_cleanWSA();
		exit(-1);
	}
#endif

	if ((*sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
	{
		printf("ERROR: create socket\n");
		_cleanWSA();
		exit(-1);
	}

	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET; // ipv4
	ServerAddr.sin_port = htons(port); // port
	ServerAddr.sin_addr.s_addr = inet_addr(ip); // server ip
	memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

	// connect
	if (connect(*sockfd, (struct sockaddr *) & ServerAddr, sizeof(struct sockaddr)) == -1)
	{
		printf("ERROR: connect");
		_cleanWSA();
		exit(-1);
	}
}

int ssend(int* sockfd, char* buf) {
#ifdef WIN32
	int Ret = send(*sockfd, buf, (int)strlen(buf), 0);
#elif __linux
	int Ret = send(*sockfd, buf, (int)strlen(buf), MSG_NOSIGNAL);
#endif
	if (Ret == -1){
		printf("ERROR: send");
	}

	return Ret;
}

void cleanup(int* sockfd) {
#ifdef WIN32
	closesocket(*sockfd);
#elif __linux
	close(*sockfd);
#endif
	_cleanWSA();
}
