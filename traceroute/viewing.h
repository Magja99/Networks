#pragma once

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>

void put(int TTL, int *num_replies, int *responsetime, struct in_addr *replies);
