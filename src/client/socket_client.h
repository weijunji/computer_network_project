#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#define SERVER
#undef SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __linux

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef SERVER

#include <netinet/in.h>

#endif

#elif WIN32

#include <windows.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

#endif

int init_socket(int* sockfd, char* ip, int port);
int ssend(int* sockfd, char* buf);
void cleanup(int* sockfd);

#endif
