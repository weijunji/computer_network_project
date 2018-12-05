#include "net.h"

int get_local_ip(char* ip) {
    #ifdef WIN32
	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(1, 1), &WSAData))
	{
		printf("initializationing error!\n");
		WSACleanup();
		exit(0);
	}
    #endif

	char hname[128];
	struct hostent *hent;

	gethostname(hname, sizeof(hname));
	hent = gethostbyname(hname);

	strcpy(ip, inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0])));
	return 0;
}

void mac_address_format(unsigned char* address, char* format) {
	sprintf(format, "%02x:%02x:%02x:%02x:%02x:%02x",
		address[0],
		address[1],
		address[2],
		address[3],
		address[4],
		address[5]
	);
}

int get_local_mac(unsigned char* mac){
    #ifdef __linux

    struct ifreq tmp;
    int sock_mac;

    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_mac == -1){
        perror("create socket fail\n");
        return -1;
    }
    memset(&tmp,0,sizeof(tmp));
    strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1 );
    if((ioctl( sock_mac, SIOCGIFHWADDR, &tmp)) < 0){
        printf("mac ioctl error\n");
        return -1;
    }
    strcpy((char*) mac, (char*) tmp.ifr_hwaddr.sa_data);
    close(sock_mac);

    #elif WIN32

	ULONG ulSize = 0;
	PIP_ADAPTER_INFO pInfo = NULL;

	GetAdaptersInfo(pInfo, &ulSize);
	pInfo = (PIP_ADAPTER_INFO)malloc(ulSize);
	GetAdaptersInfo(pInfo, &ulSize);

	strcpy((char*)mac, (char*)pInfo->Address);
	//pInfo = pInfo->Next;

    #endif
    return 0;
}

int get_remote_mac(char* ip, unsigned char* mac){
    #ifdef WIN32
    HRESULT hr;
	IPAddr  ipAddr;
	ULONG   pulMac[2];
	ULONG   ulLen;
    
	char strMacAddr[100] = { 0 };
	ipAddr = inet_addr(ip);
	memset(pulMac, 0xff, sizeof(pulMac));
	ulLen = 6;
	hr = SendARP(ipAddr, 0, pulMac, &ulLen);
	if (hr != NO_ERROR)
		return -1;
    strcpy((char*)mac, (char*) pulMac);

	#elif __linux
	mac[0] = 0xff;
	mac[1] = 0xff;
	mac[2] = 0xff;
	mac[3] = 0xff;
	mac[4] = 0xff;
	mac[5] = 0xff;
    #endif
    return 0;
}

/*
int main(){
    char ip[16];
	get_local_ip(ip);
	printf("%s\n", ip); // result: 192.168.x.x

    unsigned char mac[7];
    char format[18];
    get_local_mac(mac);
    mac_address_format(mac, format);
    printf("%s", format); // result: bc:83:85:ee:76:bc
    return 0;
}
*/
