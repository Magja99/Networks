#include "sending.h"
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

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = static_cast<const u_int16_t*>(buff);
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;

	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int send_s(int socket, int TTL, int proc_id, struct sockaddr_in recipient)
{
	struct icmp header;
	header.icmp_type = ICMP_ECHO;
	header.icmp_code = 0;
	header.icmp_id  = proc_id;
	header.icmp_seq = TTL;
	header.icmp_cksum = 0;
	header.icmp_cksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));
	setsockopt(socket, IPPROTO_IP, IP_TTL, &TTL, sizeof(TTL));
	if (sendto(socket, &header, sizeof(header), 0, (struct sockaddr*)&recipient, sizeof(recipient)) < 0)
	{
		fprintf(stderr, "Couldn't send packet: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
