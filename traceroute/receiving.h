#pragma once

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>

int receive_single(int socket, int TTL, int proc_id, struct in_addr *replies);
int receive(int socket, int TTL, int proc_id, int *num_replies, int *tim, struct in_addr *replies);
