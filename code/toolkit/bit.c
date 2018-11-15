#include <stdlib.h>
#include <stdio.h>
#include <string.h>


unsigned int bytes_to_int32(char *buffer)
{
	unsigned int pkt_size = 0;
	pkt_size |= buffer[3];
	pkt_size <<= 8;
	pkt_size |= buffer[2];
	pkt_size <<= 8;
	pkt_size |= buffer[1];
	pkt_size <<= 8;
	pkt_size |= buffer[0];
	return pkt_size;
}