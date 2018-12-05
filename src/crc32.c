#include "crc32.h"

unsigned long crc32(unsigned char* data, long length){
    unsigned long reg = 0xffffffff;
    for (int i = 0; i < length; i++){
        unsigned char add;
        add = ( (reg & 0x80000000) ? 1 : 0 ) ^ data[i];
        reg <<= 1;

        if (add) {
        reg ^= POLY;
        }
    }
    return reg;
}
