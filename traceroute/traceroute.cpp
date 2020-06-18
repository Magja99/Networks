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

#include "traceroute.h"
#include "receiving.h"
#include "sending.h"
#include "viewing.h"

bool traceroute(int socket, struct sockaddr_in *rec)
{
	int proc_id = getpid();
	for(int TTL = 1; TTL <= 30; TTL++)
	{
		int num_replies = 0;
		int tim = 0;
		struct in_addr replies[3];

		for(int i = 0; i < 3; i++)
		{
			int k = send_s(socket, TTL, proc_id, *rec);
			if(k == -1)
				break;
		}

		int ans = receive(socket, TTL, proc_id, &num_replies, &tim, replies);
		if(ans == -1)
			return false;
		put(TTL, &num_replies, &tim, replies);
		if(ans == 1)
			break;
	}
	return true;
}
