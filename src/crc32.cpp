#include "crc32.h"

unsigned int crc32_1byte(const void* data, int length, uint32_t previousCrc32)
{
  unsigned int crc = previousCrc32;
  char* current = (char*) data;

  while (length--)
  {
    char offset = char(crc) ^ *current++;
    crc = (crc >> 8) ^ crc32Lookup[offset];
  }
  return crc; 
}
