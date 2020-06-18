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
#include "send_receive.h"
#include "router.h"
#include "converters.h"
using namespace std;

void print_table(vector<route_table> &table)
{
	for(auto &i: table)
	{
		cout << i.network << " ";
		if(i.dist >= INF)
		cout << "unreachable ";
		else  cout << "distance " << i.dist << " ";
		if(i.neighbour && (i.via == "" || i.dist == INF)) cout << "connected directly";
		else cout << "via " << i.via;
		cout << endl;
	}
	cout << endl;
}

int router(int socket, vector<route_table> &table, struct sockaddr_in *rec, unordered_map<unsigned int, int> &Myself)
{
	auto start = std::chrono::steady_clock::now();
	auto end = start + chrono::seconds(15);
	int turn = 1;
	while(true)
	{
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (socket, &descriptors);
		struct timeval tv;

		int time_left = chrono::duration_cast<std::chrono::seconds>(end - std::chrono::steady_clock::now()).count();
		tv.tv_sec = time_left;
		tv.tv_usec = 0;

		int ready = select(socket + 1, &descriptors, NULL, NULL, &tv);
		if(ready == 0 || time_left < 0)
		{
			for(auto i: table)
			{
				if(i.neighbour)
				{
					string broadcast = give_addr(i.network, 0);
					//cout << "Wyslano do " << broadcast << endl;
					for(auto j: table)
					{
						if(j.via != "")
						{
							auto net_mask = ip_to_declimal(i.network);
							auto broadcast_mask = ip_to_declimal(broadcast);
							auto via_ip_mask = ip_to_declimal(j.via);
							if(net_mask.first <= via_ip_mask.first && via_ip_mask.first <= broadcast_mask.first)
							{
								continue;
							}
						}
						send_to_neighbours(socket, j, *rec, broadcast, table);
					}
				}
			}
			start = std::chrono::steady_clock::now();
			end = start + chrono::seconds(15);
			print_table(table);
			turn++;
		}
		else if(ready > 0)
			if(!receive(socket, table, turn, Myself))
			{
				cout << "Couldn't receive a packet\n";
				return 0;
			}
	}
	return 1;
}
