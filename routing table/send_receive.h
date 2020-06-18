#pragma once

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


void send_to_neighbours(int socket, route_table table, struct sockaddr_in rec, string broadcast_addr, vector<route_table> &tt);

int receive(int socket, vector<route_table> &table, int turn, unordered_map<unsigned int, int> &Myself);
