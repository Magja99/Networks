#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <chrono>
#include <sys/time.h>

#include "receiving.h"


int receive_single(int socket, int TTL, int proc_id, struct in_addr *replies)
{
	u_int8_t buffer[IP_MAXPACKET+1];

	if(recv(socket, buffer, IP_MAXPACKET, MSG_DONTWAIT) < 0)
	{
		if(errno != EAGAIN || errno !=  EWOULDBLOCK)
		{
			fprintf(stderr, "Problem with recv: %s\n", strerror(errno));
			return -1;
		}
	}

	struct ip *ipp = (struct ip *)buffer;
	struct icmp *icmpp = (struct icmp *)((uint8_t *)ipp + (*ipp).ip_hl * 4);
	if(icmpp->icmp_type == ICMP_TIMXCEED)
	{
		struct ip *ipp1 = (struct ip *)((uint8_t *)icmpp + 8);
		struct icmp *icmpp1 = (struct icmp *)((uint8_t *)ipp1 + (*ipp1).ip_hl * 4);
		if(icmpp1->icmp_seq == TTL && icmpp1->icmp_id == proc_id)
		{
			*replies = ipp -> ip_src;
			return 0;
		}
	}
	else if(icmpp -> icmp_type == ICMP_ECHOREPLY && icmpp -> icmp_seq == TTL && icmpp -> icmp_id == proc_id)
	{
		*replies=ipp -> ip_src;
		return 1;
	}
	return 2;
}

int receive(int socket, int TTL, int proc_id, int *num_replies, int *tim, struct in_addr *replies)
{
	int time_of_one = 0;
	int time_of_all = 0;
	int destination;
	auto start = std::chrono::steady_clock::now();

	while(*num_replies != 3)
	{
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (socket, &descriptors);
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int ready = select(socket + 1, &descriptors, NULL, NULL, &tv);
		if(ready == 0)
			break;
		if(ready == -1)
			return -1;

		destination = receive_single(socket, TTL, proc_id, replies+*num_replies);
		if(destination == 1 || destination == 0)
		{
			(*num_replies) ++;
			time_of_all += time_of_one;
		}
		if(destination == -1) return -1;
		auto end = std::chrono::steady_clock::now();
		time_of_one = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}

	if(*num_replies == 3)
		*tim = time_of_all / 3;
	else
		*tim = -1;

	if(destination == 1)
		return true;

	return false;
}
