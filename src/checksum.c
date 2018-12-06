#include "checksum.h"

unsigned short checksum(const void* data, int len) {
	long sum = 0;
	unsigned short* octet = (unsigned short*)data;
	while (len > 1) {
		sum += *octet++;
		len -= 2;
	}
	if (len > 0) {
		char left_over[2] = { 0 };
		left_over[0] = *octet;
		sum += *(unsigned short*)left_over;
	}

	while (sum >> 16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}
	return ~sum;
}