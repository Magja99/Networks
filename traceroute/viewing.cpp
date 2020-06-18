#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <chrono>
#include <stdbool.h>
#include <sys/time.h>

#include <assert.h>
#include <netdb.h>

#include "viewing.h"

void put(int TTL, int *num_replies, int *responsetime, struct in_addr *replies)
{
	char reply1_ip[20];
	char reply2_ip[20];
	char reply3_ip[20];

	printf("%d. ", TTL);
	inet_ntop(AF_INET, &(replies[0]),reply1_ip,sizeof(reply1_ip));
	inet_ntop(AF_INET, &(replies[1]),reply2_ip,sizeof(reply2_ip));
	inet_ntop(AF_INET, &(replies[2]),reply3_ip,sizeof(reply3_ip));

	if(*num_replies == 0) printf("*");
	else
	{
		printf("%s", reply1_ip);

		if(strcmp(reply1_ip, reply2_ip))
			printf(" %s", reply2_ip);

		if(strcmp(reply1_ip ,reply3_ip) && strcmp(reply2_ip, reply3_ip))
			printf(" %s", reply3_ip);

		if(*responsetime == -1) printf("\t???");
		else printf(" %dms", *responsetime);
	}
	printf("\n");
}
