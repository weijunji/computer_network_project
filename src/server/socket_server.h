#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __linux

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#elif WIN32

#include <windows.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

#endif

#define BACKLOG 10

void init_socket(int* sockfd, int port);
int recieve(int* sockfd, char* buf, int size);

#endif