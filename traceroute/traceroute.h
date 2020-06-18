#pragma once

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>

bool traceroute(int socket, struct sockaddr_in *rec);
