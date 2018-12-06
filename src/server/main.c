#include "socket_server.h"
#include <string.h>
#include "../crc32.h"
#include "../checksum.h"
#include "../sk_buff.h"
#include "../net.h"
#include <stdlib.h>
#include <stdio.h>

// 打印出可见字符，将不可见字符打印为 .
void print_char(const unsigned char* data, int len){
	for(int i = 0; i < len; i++){
		if(i != 0 && i % 4 == 0){
			putchar(' ');
		}
		if(data[i] >= 32 && data[i] <= 126){
			putchar(data[i]);
		}else{
			putchar('.');
		}
	}
	putchar('\n');
}

/* Transport layer */

void transport_layer(struct sk_buff *skb){
	printf("\nTransport layer:\n");
	printf("Data:\n");
	print_char(skb->data, skb->len);
}

/* End of transport layer */

/* Network layer */

struct ipv4_header {
	unsigned char	version : 4;
	unsigned char	header_length : 4;
	unsigned char	service;
	unsigned short	total_length;
	unsigned short	identifier;
	unsigned short	flag : 3;
	unsigned short	offset : 13;
	unsigned char	ttl;
	unsigned char	protocol;
	unsigned short	checksum;
	unsigned int 	source;
	unsigned int 	dest;
};

void ipv4(struct sk_buff* skb){
	printf("IPv4:\n");
	struct ipv4_header* header = (struct ipv4_header*) malloc(sizeof(struct ipv4_header));
	struct in_addr in;

	memcpy(header, skb->data, 20);
	skb_pull(skb, header->header_length * 4);
	printf("Header:");

	memcpy(&in, &(header->dest), 4);
	char* ip = inet_ntoa(in);
	printf("\tDest ip:\t%s\n", ip);
	memcpy(&in, &(header->source), 4);
	ip = inet_ntoa(in);
	printf("\tSource ip:\t%s\n", ip);
	
	unsigned short sum = checksum((void*)header, 20);
	printf("\tChecksum:\t%x\n", sum);
	if(sum != 0){
		printf("Checksum ERROR: Drop");
		return;
	}

	free(header);
	transport_layer(skb);
}

void network_layer(struct sk_buff* skb)
{
	printf("\nNetwork layer: \n");
	printf("Data:\n");
	print_char(skb->data, skb->len);

	if(skb->protocol == 0x0800){
		ipv4(skb);
	}else{
		printf("Cannot solve protocol: %x\n", skb->protocol);
		free_skb(skb);
	}
}

/* End of network layer */

/* Data link layer */

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

union btb {
	char c;
	struct bits bits_;
};

struct ethernet_header{
	unsigned char dest_mac[6];
	unsigned char sour_mac[6];
	unsigned short length;
	unsigned short type; // 0800 IPv4; 0806 ARP; 86DD IPv6
};

void data_link_layer(unsigned char *frame, unsigned int len){
	int i;
	unsigned long cc;

	printf("\nData link layer:\n");
	printf("Data:\n");
	for(int i = 0; i < len; i++){
		if(i != 0 && i % 8 == 0) printf(" ");
		printf("%d", frame[i]);
	}
	printf("\n");

	cc = crc32(frame, len);
	printf("CRC result: 0x%x\n", cc);
	if((int)cc != 0){
		printf("CRC ERROR: Drop this frame\n");
		return;
	}
	unsigned char* data;
	data = (unsigned char*) malloc(sizeof(char) * (len - 32) / 8);

	for(i = 0; i < (len-32) / 8; i++){
		union btb btb_;
		btb_.bits_.bit8 = frame[i * 8];
		btb_.bits_.bit7 = frame[i * 8 + 1];
		btb_.bits_.bit6 = frame[i * 8 + 2];
		btb_.bits_.bit5 = frame[i * 8 + 3];
		btb_.bits_.bit4 = frame[i * 8 + 4];
		btb_.bits_.bit3 = frame[i * 8 + 5];
		btb_.bits_.bit2 = frame[i * 8 + 6];
		btb_.bits_.bit1 = frame[i * 8 + 7];
		data[i] = btb_.c;
	}

	struct ethernet_header* header = (struct ethernet_header*) malloc(sizeof(struct ethernet_header));
	memcpy((char*)header, data + 8, 16);
	printf("Header:");
	printf("\tdest mac:\t");
	print_mac_address(header->dest_mac);
	printf("\n\tsour mac:\t");
	print_mac_address(header->sour_mac);
	printf("\n\tlength:\t\t%d\n", header->length);
	printf("\tprotocol:\t%04x\n", header->type);

	unsigned char my_addr[6];
	get_local_mac(my_addr);
	unsigned char broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	if(memcmp((char*)header->dest_mac, (char*)my_addr, 6) != 0 && memcmp((char*)header->dest_mac, (char*)broadcast, 6) != 0){
		printf("此帧目的地址非本机，舍去\n");
		return;
	}

	struct sk_buff *skb = alloc_skb(header->length);
	skb_put(skb, data + 24, header->length);
	skb->protocol = header->type;

	network_layer(skb);
	free(data);
}

/* End of data link layer */

/* Physical layer */

void physical_layer(char* data){
	int size = strlen(data) - 1;
	char* updata;
	updata = (char*) malloc(sizeof(char) * (size / 8 + 1));

	printf("Physical layer:\n");
	printf("Length: %d\n", size);
	printf("Data:\n");
	// 将-1转为0
	for(int i = 0; i < size; i++){
		if(i != 0 && i % 8 == 0){
			printf(" ");
		}
		if(data[i] == -1){
			printf("0");
			data[i] = 0;
		}else{
			printf("1");
		}
	}
	printf("\n");
	data_link_layer(data, size);
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
