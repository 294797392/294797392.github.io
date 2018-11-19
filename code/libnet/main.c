#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "network_driver.h"

extern struct network_driver_s network_driver_ndis;

int main1(int agrc, char *argv[])
{
	network_driver_ndis.init("");

	return 1;
}