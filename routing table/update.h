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

using namespace std;

bool in_range(unsigned int net, unsigned int via, unsigned int broad);

void set_unreachable(string net, vector<route_table> &table, int turn);

void erase_unreachable(vector<route_table> &table, int turn);

int update(datagram *new_component, vector<route_table> &table, string via_ip, int turn);
