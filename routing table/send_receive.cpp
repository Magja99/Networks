#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <chrono>
#include <stdbool.h>
#include <sys/time.h>
#include <cstdio>
#include <assert.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <iostream>
#include <cstring>
#include <bitset>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>

#include "types.h"
#include "converters.h"
#include "update.h"
#include "send_receive.h"

void send_to_neighbours(int socket, route_table table, struct sockaddr_in rec, string broadcast_addr, vector<route_table> &tt)
{
	datagram msg = convert_to_datagram(table.network, table.dist);
	inet_pton(AF_INET, broadcast_addr.c_str(), &rec.sin_addr);
	if (sendto(socket, &msg, sizeof(msg), 0, (struct sockaddr*) &rec, sizeof(rec)) !=sizeof(msg))
	{
		for(auto &i: tt)
		{
			if(give_addr(i.network, 0) == broadcast_addr && i.via == "")
				i.dist = INF;
		}
	}
	else
	{
		for(auto &i: tt)
		{
			if(give_addr(i.network, 0) == broadcast_addr && i.dist >= INF)
			{
				i.dist = i.neighbour_dist;
				i.via = "";
			}
		}
	}
}

int receive(int socket, vector<route_table> &table, int turn, unordered_map<unsigned int, int> &Myself)
{
	struct sockaddr_in  sender;
	socklen_t           sender_len = sizeof(sender);
	datagram buffer[1];

	ssize_t datagram_len = recvfrom (socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender, &sender_len);
	if (datagram_len < 0) {
		return 0;
	}

	char sender_ip_str[20];
	inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

	pair<unsigned int, int> k = ip_to_declimal(sender_ip_str);
	if(Myself.find(k.first) != Myself.end())
		return 1;

	update(buffer, table, sender_ip_str, turn);

	fflush(stdout);
	return 1;
}
