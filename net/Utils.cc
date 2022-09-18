#include "Utils.hh"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

using namespace utils;

void utils::hexdump(const char* addr, size_t buflen)
{
    unsigned char* buf = (unsigned char*)addr;
    unsigned int i = 0, j = 0;

    printf("hexdump begin...\n");
    for (i = 0; i < buflen; i += 16) {
        printf("%06x: ", i);
        for (j = 0; j < 16; j++) 
        if (i+j < buflen)
            printf("%02x ", buf[i+j]);
        else
            printf("   ");
        printf(" ");
        for (j = 0; j < 16; j++) 
        if (i+j < buflen)
            printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
        printf("\n");
    }
    printf("hexdump end\n");
}