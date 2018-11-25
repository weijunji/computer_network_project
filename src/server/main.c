#include "socket_server.h"
#include <string.h>

/* Data link layer */

void data_link_layer(char* data){
	printf("Data link layer:\t");
	printf("%s", data);
}

/* End of data link layer */

/* Physical layer */

// 通过位域转换二进制
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
	int size = strlen(data);
	char* updata;
	updata = (char*) malloc(sizeof(char) * (size / 8 + 1));

	printf("Physical layer:\t");
	// 将-1转为0
	for(int i = 0; i < size; i++){
		if(data[i] == -1){
			printf("0");
			data[i] = 0;
		}
		else printf("1");
	}
	printf("\n");
	for(int i = 0; i < size / 8; i++){
		union btb btb_;
		btb_.bits_.bit1 = data[i * 8];
		btb_.bits_.bit2 = data[i * 8 + 1];
		btb_.bits_.bit3 = data[i * 8 + 2];
		btb_.bits_.bit4 = data[i * 8 + 3];
		btb_.bits_.bit5 = data[i * 8 + 4];
		btb_.bits_.bit6 = data[i * 8 + 5];
		btb_.bits_.bit7 = data[i * 8 + 6];
		btb_.bits_.bit8 = data[i * 8 + 7];
		updata[i] = btb_.c;
	}
	updata[size / 8] = '\0';

	printf("up");
	data_link_layer(updata);
	free(updata);
}

/* End of physical layer */

int main()
{
	int port = 9628;

	char buf[2000] = "hello world";
	int* iServerSock;
	iServerSock = (int*)malloc(sizeof(int));

	init_socket(iServerSock, port);

	while (true)
	{
		recieve(iServerSock, buf, 2000);
		/*
		for(int i = 0; i < 16; i++){
			printf("%d", (int) buf[i]);
		}
		*/
		physical_layer(buf);
	}
}
