#include "socket_client.h"
#include <string.h>


// !!! char* 必须以 \0 结尾


/* Physical layer */

struct bits{
	char bit1 : 1;
	char bit2 : 1;
	char bit3 : 1;
	char bit4 : 1;
	char bit5 : 1;
	char bit6 : 1;
	char bit7 : 1;
	char bit8 : 1;
};

union btb{
	char c;
	struct bits bits_;
};

void physical_layer(char* data){
	// convert to bit stream
	char* bit_stream;
	bit_stream = (char*) malloc(sizeof(char) * (strlen(data) * 8 + 1));
	for(int i = 0; i < strlen(data); i++){
		union btb btb_;
		btb_.c = data[i];
		bit_stream[i * 8] = btb_.bits_.bit1;
		bit_stream[i * 8 + 1] = btb_.bits_.bit2;
		bit_stream[i * 8 + 2] = btb_.bits_.bit3;
		bit_stream[i * 8 + 3] = btb_.bits_.bit4;
		bit_stream[i * 8 + 4] = btb_.bits_.bit5;
		bit_stream[i * 8 + 5] = btb_.bits_.bit6;
		bit_stream[i * 8 + 6] = btb_.bits_.bit7;
		bit_stream[i * 8 + 7] = btb_.bits_.bit8;
	}
	printf("Physical layer:\t");

	// char = 0 会导致字符串结尾判断错误，故改为 -1 和 1
	for(int i = 0; i < strlen(data) * 8; i++){
		if(bit_stream[i] == 0){
			printf("0");
			bit_stream[i] = -1;
		}else{
			printf("1");
			bit_stream[i] = 1;
		}
	}
	printf("\n");
	bit_stream[strlen(data) * 8] = '\0';

	char* ip = "192.168.31.178";
	int port = 9628;
	int* sockfd = (int*)malloc(sizeof(int));

	init_socket(sockfd, ip, port);
	ssend(sockfd, bit_stream);

	free(sockfd);
	cleanup(sockfd);
}

/* End of physical layer */

int main()
{
	char* s = "ha";
	physical_layer(s);

	/* Example of socket
	char buf[2000] = "hello world";
	char* ip = "192.168.31.178";
	int port = 9628;
	int* sockfd = (int*)malloc(sizeof(int));

	for (int i = 0; i < 5; i++) {
		init_socket(sockfd, ip, port);
		ssend(sockfd, buf);
	}

	cleanup(sockfd);
	*/
}
