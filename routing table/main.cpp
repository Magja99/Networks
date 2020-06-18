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

#include "router.h"
#include "types.h"
#include "converters.h"
using namespace std;

int main()
{
	int n;
	cin >> n;

	vector<route_table> distance_vector;
	unordered_map<unsigned int, int> Myself;

	for(int i = 0; i < n; i++)
	{
		string addr, s;
		unsigned int di;
		cin >> addr >> s >> di;
		route_table x = {give_addr(addr, 1), di, "", 1, 0, 0, di};
		distance_vector.push_back(x);
		pair<unsigned int, int> k = ip_to_declimal(addr);
		Myself[k.first] = 1;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)	{
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	int broadcastPermission=1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,(void *)&broadcastPermission,sizeof(broadcastPermission));
	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(54321);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (::bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if(!router(sockfd, distance_vector, &server_address, Myself))
		return EXIT_FAILURE;
}
