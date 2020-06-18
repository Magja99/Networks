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

using namespace std;

const unsigned int INF = 4294967295;
int n;

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

unordered_map<unsigned int, int> Myself;

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
void set_unrechable(string net, vector<route_table> &table, int turn) // bierze broadcast i ustawia wszystkie ip z nim związane na unreachable
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

void print_table(vector<route_table> &table, int turn)
{
	for(auto &i: table)
	{
		cout << i.network << " ";
		if(i.dist >= INF)
		cout << "unreachable ";
		else  cout << "distance " << i.dist << " ";
		if(i.neighbour && (i.via == "" || i.dist == INF)) cout << "connected directly";
		else cout << "via " << i.via;
		//cout << " turn: " << turn << " un.tur: " << i.unreachable_tur << endl;
		cout << endl;
	}
	cout << endl;
}

void erase_unrechable(vector<route_table> &table, int turn)
{
	queue <int> Q;
	for(int i = 0; i < table.size(); i++)
		if(table[i].dist >= INF && !table[i].neighbour && turn - table[i].unreachable_tur >= 3)
		{
			Q.push(i);
		//	cout << "erasing: " << i << endl;
		}

	while(!Q.empty())
	{
		table.erase(table.begin() + Q.front());
		Q.pop();
	}
}

int update(datagram *new_component, vector<route_table> &table, string via_ip, int turn)
{
	unsigned int n_c_distance = new_component->dist;
	unsigned int rec_distance = 0;
	for(auto &i: table) // aktualizowanie unreachable przez tury
		if(turn - i.tur >= 3 && i.neighbour)
			set_unrechable(give_addr(i.network, 0), table, turn);

	erase_unrechable(table, turn);

	for(auto &i: table) // odzyskiwanie drogi do sąsiada
	{
		auto net_mask = ip_to_declimal(i.network);
		auto broadcast_mask = ip_to_declimal(give_addr(i.network, 0));
		auto via_ip_mask = ip_to_declimal(via_ip);
		if(net_mask.first <= via_ip_mask.first && via_ip_mask.first <= broadcast_mask.first)
		{
			rec_distance = i.dist;
			i.tur = turn;
			break;
		}
	}
	string net = to_string(new_component->a) + '.' + to_string(new_component->b) + '.' + to_string(new_component->c) +'.'
				 + to_string(new_component->d) + '/' + to_string(new_component->m); // ip do nowej sieci

	//cout << "Update: " << net << " " << n_c_distance << endl;
	bool byl = 0;
	for(auto &i: table)
	{
		auto net_mask = ip_to_declimal(i.network);
		auto broadcast_mask = ip_to_declimal(give_addr(i.network, 0));
		auto via_ip_mask = ip_to_declimal(via_ip);
		if(i.network == net && i.neighbour && net_mask.first <= via_ip_mask.first && via_ip_mask.first <= broadcast_mask.first)
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
			break;
		}
	}

	if(!byl && n_c_distance < INF)
		table.push_back({net, n_c_distance + rec_distance, via_ip, 0, turn, 0, 0});
	return 0;
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

int send_to_neighbours(int socket, route_table table, struct sockaddr_in rec, string broadcast_addr, vector<route_table> &tt, int turn)
{
	unsigned int distance = table.dist;
	datagram msg = convert_to_datagram(table.network, table.dist);
	inet_pton(AF_INET, broadcast_addr.c_str(), &rec.sin_addr);
	if (sendto(socket, &msg, sizeof(msg), 0, (struct sockaddr*) &rec, sizeof(rec)) !=sizeof(msg))
	{
		//fprintf(stderr, "sendto error: %s\n", strerror(errno));
		for(auto &i: tt)
		{
			if(give_addr(i.network, 0) == broadcast_addr)
				i.dist = INF;
		}
		return EXIT_FAILURE;
	}
	return 0;
}

int receive(int socket, vector<route_table> &table, int turn)
{
	struct sockaddr_in  sender;
	socklen_t           sender_len = sizeof(sender);
	datagram buffer[1];

	ssize_t datagram_len = recvfrom (socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender, &sender_len);
	if (datagram_len < 0) {
		//fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	char sender_ip_str[20];
	inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

	pair<unsigned int, int> k = ip_to_declimal(sender_ip_str);
	if(Myself.find(k.first) != Myself.end())
		return 1;

	//printf ("Received UDP packet from IP address: %s, port: %d\n", sender_ip_str, ntohs(sender.sin_port));
	update(buffer, table, sender_ip_str, turn);

	fflush(stdout);
	return 0;
}

void router(int socket, vector<route_table> &table, struct sockaddr_in *rec)
{
	auto start = std::chrono::steady_clock::now();
	auto end = start + chrono::seconds(20);
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
						send_to_neighbours(socket, j, *rec, broadcast, table, turn);
					}
				}
			}
			start = std::chrono::steady_clock::now();
			end = start + chrono::seconds(20);
			print_table(table, turn);
			turn++;
		}
		else if(ready > 0)
			receive(socket, table, turn);
	}
}
int main()
{
	cin >> n;

	vector<route_table> distance_vector;

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

	router(sockfd, distance_vector, &server_address)
}
