#ifndef CRC32_H
#define CRC32_H

#define POLY 0x4c11db7

unsigned long crc32(unsigned char* data, long length);

#endif