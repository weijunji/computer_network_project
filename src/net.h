#ifndef NET_H
#define NET_H

#ifdef __linux

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#elif WIN32

#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

#endif

#include <stdio.h>
#include <string.h>

int get_local_ip(char* ip); // 获取本机ip
void mac_address_format(unsigned char* address, char* format); // 转换为16进制冒号格式
int get_local_mac(unsigned char* mac); // 获取本机mac地址 -> uc* mac
int get_remote_mac(char* ip, char* mac); // 根据ip获取mac地址

#endif