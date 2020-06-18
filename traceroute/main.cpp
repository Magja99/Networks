#include "traceroute.h"
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
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


int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Bad IP\n");
		return EXIT_FAILURE;
	}

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	struct sockaddr_in recipient;
	recipient.sin_family = AF_INET;
	recipient.sin_port = htons(7);

	if(inet_pton(AF_INET, argv[1], &recipient.sin_addr) == 0)
	{
		fprintf(stderr, "Could't enter the recipient's address into the address structure:\n");
		return EXIT_FAILURE;
	}

	if(!traceroute(sockfd, &recipient)) return EXIT_FAILURE;
}
