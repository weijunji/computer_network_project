#include "socket_client.h"
#include "../sk_buff.h"
#include "../net.h"
#include "../crc32.h"
#include "../checksum.h"
#include <string.h>

void physical_layer(char *bit_stream, unsigned int len, char* ip, int port);
void data_link_layer(struct sk_buff *skb, unsigned char* dest_mac);

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

/* Application layer */

struct application_layer_header
{
	char h;
};

void application_layer(char *data, char *dest_ip)
{
	
}

/* End of application layer */

/* Transport layer */

void transport_layer(struct sk_buff *skb)
{
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
	unsigned char	protocol; // 1 ICMP; 2 IGMP; 6 TCP; 17 UPD;
	unsigned short	checksum;
	unsigned int 	source;
	unsigned int 	dest;
};

void network_layer(struct sk_buff *skb)
{
	printf("\nNetwork layer:\n");
	printf("Data:\n");
	print_char(skb->data, skb->len);

	int size = sizeof(struct ipv4_header);
	struct ipv4_header* header = (struct ipv4_header*) malloc(sizeof(struct ipv4_header));
	header->version = 4;
	header->header_length = size / 4;
	header->service = 0;
	header->total_length = size + skb->len;
	header->identifier = 0;
	header->flag = 0;
	header->offset = 0;
	header->ttl = 1;
	header->protocol = skb->protocol;
	header->checksum = 0;

	char source_ip[16];
	get_local_ip(source_ip);
	printf("Source ip:\t%s\n", source_ip);
	header->source = inet_addr(source_ip);
	printf("Dest ip:\t%s\n", skb->dest_ip);
	header->dest = inet_addr(skb->dest_ip);
	//printf("%s", source_ip);

	header->checksum = checksum((void*)header, size);
	printf("Checksum:\t0x%x\n", header->checksum);
	// printf("%x\n", checksum((void*)header, size));

	skb_push(skb, (unsigned char*)header, size);
	skb->protocol = 0x0800; // IP Protocol

	unsigned char* dest_mac = (unsigned char*) malloc(sizeof(char) * 6);
	get_remote_mac(skb->dest_ip, dest_mac);

	data_link_layer(skb, dest_mac);
	free(dest_mac);
}

/* End of network layer */

/* Data link layer */
#define PREAMBLE 170 //前导码10101010
#define SOF 171	//定界符10101011

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

struct ethernet_head{
	unsigned char dest_mac[6];
	unsigned char sour_mac[6];
	unsigned short length;
	unsigned short type; // 0800 IPv4; 0806 ARP; 86DD IPv6
};

void data_link_layer(struct sk_buff *skb, unsigned char* dest_mac)
{
	printf("\nData link layer:\n");
	printf("Data: \n");
	print_char(skb->data, skb->len);

	struct ethernet_head* head = (struct ethernet_head*) malloc(sizeof(struct ethernet_head));

	memcpy(head->dest_mac, dest_mac, 6);

	get_local_mac(head->sour_mac);

	head->length = skb->len;

	head->type = skb->protocol; // TODO: GET FROM SKB->PROTOCOL

	printf("Header:");
	printf("\tdest mac:\t");
	print_mac_address(head->dest_mac);
	printf("\n\tsour mac:\t");
	print_mac_address(head->sour_mac);
	printf("\n\tlength:\t\t%d\n", head->length);
	printf("\tprotocol:\t%04x\n", head->type);
	printf("\n");

	// 填充
	if (skb->len < 46){
		for (int i = 0; i < 46 - skb->len; i++){
			skb_put(skb, (unsigned char *)" ", 1);
		}
	}

	// 前导码 + 定界符
	unsigned char frame_h[8] = {0};
	for (int i = 0; i < 7; i++)
		frame_h[i] = PREAMBLE;
	frame_h[7] = SOF;

	unsigned char* headc = (unsigned char*) head;

	skb_push(skb, headc, 16);
	skb_push(skb, frame_h, 8);

	unsigned char *bit_stream;
	bit_stream = (unsigned char *)malloc(sizeof(char) * ((skb->len + 4) * 8));

	for (int i = 0; i < skb->len; i++)
	{
		union btb btb_;
		btb_.c = skb->data[i];
		bit_stream[i * 8] = btb_.bits_.bit8; //从高位到低位
		bit_stream[i * 8 + 1] = btb_.bits_.bit7;
		bit_stream[i * 8 + 2] = btb_.bits_.bit6;
		bit_stream[i * 8 + 3] = btb_.bits_.bit5;
		bit_stream[i * 8 + 4] = btb_.bits_.bit4;
		bit_stream[i * 8 + 5] = btb_.bits_.bit3;
		bit_stream[i * 8 + 6] = btb_.bits_.bit2;
		bit_stream[i * 8 + 7] = btb_.bits_.bit1;
	}

	for(int i = 0; i < skb->len * 8; i++){
		if(bit_stream[i] != 0){
			bit_stream[i] = 1;
		}
	}

	unsigned long cc;
	cc = crc32(bit_stream, skb->len * 8);

	printf("CRC result: %x\n", cc);

	unsigned char a[4];
	a[0] = (cc >> 24) & 0xff;
	a[1] = (cc >> 16) & 0xff;
	a[2] = (cc >> 8) & 0xff;
	a[3] = cc & 0xff;

	for (int i = skb->len, j = 0; i < skb->len + 4; i++, j++){
		union btb btb_;
		btb_.c = a[j];
		bit_stream[i * 8] = btb_.bits_.bit8;
		bit_stream[i * 8 + 1] = btb_.bits_.bit7;
		bit_stream[i * 8 + 2] = btb_.bits_.bit6;
		bit_stream[i * 8 + 3] = btb_.bits_.bit5;
		bit_stream[i * 8 + 4] = btb_.bits_.bit4;
		bit_stream[i * 8 + 5] = btb_.bits_.bit3;
		bit_stream[i * 8 + 6] = btb_.bits_.bit2;
		bit_stream[i * 8 + 7] = btb_.bits_.bit1;
	}

	physical_layer((char*)bit_stream, (skb->len + 4) * 8, skb->dest_ip, skb->dest_port);
	free(bit_stream);
}

/* End of data link layer */

/* Physical layer */

void physical_layer(char *bit_stream, unsigned int len, char* ip, int port)
{
	printf("\nPhysical layer: \n");
	printf("Send:\n");
	for(int i = 0; i < len; i++){
		if(i != 0 && i % 8 == 0){
			printf(" ");
		}
		if (bit_stream[i] == 0){
			bit_stream[i] = -1;
			printf("0");
		}else{
			bit_stream[i] = 1;
			printf("1");
		}
	}
	printf("\n");
	bit_stream[len] = '\0';

	int *sockfd = (int *)malloc(sizeof(int));

	init_socket(sockfd, ip, port);
	ssend(sockfd, (char *)bit_stream);

	free(sockfd);
	cleanup(sockfd);
}

/* End of physical layer */

int main(){
	char *s = "skdskdfbksj";
	// physical_layer(s);

	struct sk_buff *skb = alloc_skb(20);
	skb_put(skb, (unsigned char*)s, 11);
	char* ip = "192.168.31.36"; // 241F A8C0
	skb->dest_ip = ip;
	skb->dest_port = 9628;
	unsigned char dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	// data_link_layer(skb, dest_mac);
	network_layer(skb);
}
