#pragma once

unsigned long Crc32(unsigned char *buf, unsigned long len);
uint16_t Crc16(const uint8_t *data, uint16_t size);

unsigned int MurmurHash2(const void * key, int len, unsigned int seed);
unsigned int MurmurHash2A(const void * key, int len, unsigned int seed);
uint64_t MurmurHash64A(const void * key, int len, unsigned int seed);
