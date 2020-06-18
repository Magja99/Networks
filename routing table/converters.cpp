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

pair<unsigned int, int> ip_to_declimal(string ip)
{
	int IP[5];
	char ch;
	stringstream s(ip);
	s >> IP[0] >> ch >> IP[1] >> ch >> IP[2] >> ch >> IP[3] >> ch >> IP[4];
	unsigned int identifier = 0;
	identifier = ((IP[0]*256 + IP[1])*256 + IP[2])*256 + IP[3];
	s.str("");
	return {identifier, IP[4]};
}

string give_addr(string ip, bool which)
{
	pair<unsigned int, int> identifier = ip_to_declimal(ip);
	bitset<32> subnet;
	for(int i = 0; i <= identifier.second; i++)
		subnet[32 - i] = 1;
	unsigned int net = identifier.first & (subnet.to_ulong());
	subnet = ~subnet;
	unsigned int mask = subnet.to_ulong();

	unsigned int broadcast = identifier.first | mask;
	unsigned int convert = broadcast;
	if(which)
		convert =  net;
	unsigned char bytes[4];
	bytes[0] = convert & 0xFF;
	bytes[1] = (convert >> 8) & 0xFF;
	bytes[2] = (convert >> 16) & 0xFF;
	bytes[3] = (convert >> 24) & 0xFF;

	string ans = to_string(bytes[3]) +'.'+ to_string(bytes[2]) +'.'+ to_string(bytes[1]) +'.'+ to_string(bytes[0]);
	if(which)
	ans += '/'+to_string(identifier.second);
	return ans;
}

datagram convert_to_datagram(string ip, unsigned int distance)
{
	int IP[5];
	char ch;
	stringstream s(ip);
	s >> IP[0] >> ch >> IP[1] >> ch >> IP[2] >> ch >> IP[3] >> ch >> IP[4];
	s.str("");

	datagram ans = {distance, (uint8_t)IP[0], (uint8_t)IP[1], (uint8_t)IP[2], (uint8_t)IP[3], (uint8_t)IP[4]};
	return ans;
}
