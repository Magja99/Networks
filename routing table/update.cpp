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

#include "converters.h"
#include "types.h"
#include "update.h"

using namespace std;

void set_unreachable(string net, vector<route_table> &table, int turn)
{
	for(auto &i: table)
	{
		if(give_addr(i.network, 0) == net)
		{
			auto net_mask = ip_to_declimal(i.network);
			auto broadcast_mask = ip_to_declimal(give_addr(i.network, 0));
			for(auto &j: table)
			{
				if(!j.neighbour)
				{
					auto via_ip_mask = ip_to_declimal(j.via);
					if(net_mask.first <= via_ip_mask.first && via_ip_mask.first <= broadcast_mask.first)
					{
						if(j.dist < INF)
							j.unreachable_tur = turn;
						j.dist = INF;
					}
				}
			}
		}
	}
}

void erase_unreachable(vector<route_table> &table, int turn)
{
	queue <int> Q;
	for(int i = 0; i < (int)table.size(); i++)
		if(table[i].dist >= INF && !table[i].neighbour && turn - table[i].unreachable_tur >= 3)
			Q.push(i);

	while(!Q.empty())
	{
		table.erase(table.begin() + Q.front());
		Q.pop();
	}
}

bool in_range(unsigned int net, unsigned int via, unsigned int broad)
{
	if(net <= via && via <= broad)
		return true;
	return false;
}

int update(datagram *new_component, vector<route_table> &table, string via_ip, int turn)
{
	unsigned int n_c_distance = new_component->dist;
	unsigned int rec_distance = 0;
	for(auto &i: table) // aktualizowanie unreachable przez tury
		if(turn - i.tur >= 3 && i.neighbour)
			set_unreachable(give_addr(i.network, 0), table, turn);

	erase_unreachable(table, turn);

	for(auto &i: table)
	{
		auto net_mask = ip_to_declimal(i.network);
		auto broadcast_mask = ip_to_declimal(give_addr(i.network, 0));
		auto via_ip_mask = ip_to_declimal(via_ip);
		if(in_range(net_mask.first, via_ip_mask.first, broadcast_mask.first))
		{
			rec_distance = i.dist;
			i.tur = turn;
			break;
		}
	}

	string net = to_string(new_component->a) + '.' + to_string(new_component->b) + '.' + to_string(new_component->c) +'.'
				 + to_string(new_component->d) + '/' + to_string(new_component->m);
	bool byl = 0;
	for(auto &i: table)
	{
		auto net_mask = ip_to_declimal(i.network);
		auto broadcast_mask = ip_to_declimal(give_addr(i.network, 0));
		auto via_ip_mask = ip_to_declimal(via_ip);
		if(i.dist > 30)
			i.dist = INF;
		if(i.network == net && i.neighbour && in_range(net_mask.first, via_ip_mask.first, broadcast_mask.first))
		{
			byl = 1;
			i.dist = i.neighbour_dist;
			i.via = "";
		}
		else
		if(i.network == net && i.via == via_ip)
		{
			byl = 1;
			if(i.dist != INF)
				i.unreachable_tur = turn;
			if((unsigned long long)n_c_distance + rec_distance >= (unsigned long long)INF)
				i.dist = INF;
			else i.dist = n_c_distance + rec_distance;

		}
		else
		if(i.network == net)
		{
			byl = 1;
			if(i.dist > (unsigned long long)n_c_distance + rec_distance)
			{
				i.via = via_ip;
				if(i.dist != INF)
					i.unreachable_tur = turn;
				if(n_c_distance >= INF){
					i.dist = INF;
				}
				else i.dist = n_c_distance + rec_distance;
			}
		}
	}
	if(!byl && n_c_distance < INF)
		table.push_back({net, n_c_distance + rec_distance, via_ip, 0, turn, 0, 0});
	return 0;
}
