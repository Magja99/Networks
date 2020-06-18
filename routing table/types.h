#pragma once
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

const unsigned int INF = 4294967295;

#pragma pack (push, 1)
struct datagram
{
	unsigned int dist;
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t m;
};
#pragma pack(pop)

struct route_table
{
	string network;
	unsigned int dist;
	string via;
	bool neighbour;
	int tur;
	int unreachable_tur;
	unsigned int neighbour_dist;
};
