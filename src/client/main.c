#include "socket_client.h"
#include "../sk_buff.h"
#include "../net.h"
#include "../crc32.h"
#include <string.h>

void physical_layer(char *bit_stream, unsigned int len, char* ip, int port);

// !!! char* 必须以 \0 结尾

/* Application layer */
/*
struct application_layer_header
{
	char h;
};

void application_layer(char *data, char *dest_ip)
{
	struct sk_buff *skb = (struct sk_buff *)malloc(sizeof(struct sk_buff));
	skb->dest_ip = dest_ip;
	skb->dest_port = 9628;

	skb_put(skb, (unsigned char*)data, strlen(data));
}
*/
/* End of application layer */

/* Transport layer */

void transport_layer(struct sk_buff *skb)
{
}

/* End of transport layer */

/* Network layer */

void network_layer(struct sk_buff *skb)
{
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

void data_link_layer(struct sk_buff *skb)
{
	struct ethernet_head* head = (struct ethernet_head*) malloc(sizeof(struct ethernet_head));

	// unsigned char* dest_mac = (unsigned char*) malloc(sizeof(char) * 6);
	get_remote_mac(skb->dest_ip, head->dest_mac);  // TODO: PUT TO IP
	// head->dest_mac = dest_mac;

	// unsigned char* local_mac = (unsigned char*) malloc(sizeof(char) * 6);
	get_local_mac(head->sour_mac);
	// head->sour_mac = local_mac;

	head->length = skb->len;

	head->type = 0x0800;

	// 填充
	if (skb->len < 46){
		for (int i = 0; i < 46 - skb->len; i++){
			skb_put(skb, (unsigned char *)" ", 1);
		}
	}

	// 前导码 + 定界符
	unsigned char frame_h[9] = {0};
	for (int i = 0; i < 7; i++)
		frame_h[i] = PREAMBLE;
	frame_h[7] = SOF;

	unsigned char* headc = (unsigned char*) head;
	skb_push(skb, headc, 16);
	
	//skb_push(skb, my_addre, 6);
	//skb_push(skb, des_addre, 6);
	skb_push(skb, frame_h, 8);
	char *bit_stream;
	bit_stream = (char *)malloc(sizeof(char) * ((skb->len + 4) * 8) + 1);

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

	unsigned long cc;
	cc = crc32(skb->data, skb->len);
	unsigned char a[4];
	a[0] = (cc >> 24) & 0xff;
	a[1] = (cc >> 16) & 0xff;
	a[2] = (cc >> 8) & 0xff;
	a[3] = cc & 0xff;

	for (int i = skb->len, j = 0; i < skb->len + 4; i++, j++)
	{
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

	printf("数据链路层封装好的帧为：\n");
	/*
	for (int i = 0; i < (skb->len + 4) * 8; i++){
		if (bit_stream[i] == 0){
			bit_stream[i] = -1; // TODO: unsigned -1?
		}else{
			bit_stream[i] = 1;
		}
	}
	*/
	physical_layer(bit_stream, (skb->len + 4) * 8, skb->dest_ip, skb->dest_port);
	free(bit_stream);
}

/* End of data link layer */

/* Physical layer */

void physical_layer(char *bit_stream, unsigned int len, char* ip, int port)
{
	/*
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
	*/
	printf("Physical layer: \n");
	for(int i = 0; i < len; i++){
		if (bit_stream[i] == 0){
			bit_stream[i] = 1;
			printf("1");
		}else{
			bit_stream[i] = 2;
			printf("0");
		}
		if(i != 0 && i % 8 == 0){
			printf(" ");
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
	char *s = "ha";
	// physical_layer(s);

	struct sk_buff *skb = alloc_skb(20);
	skb_put(skb, (unsigned char*)s, 2);
	char* ip = "192.168.31.36";
	skb->dest_ip = ip;
	skb->dest_port = 9628;
	data_link_layer(skb);
}
