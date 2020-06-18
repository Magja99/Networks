#pragma once

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

int send_s(int socket, int TTL, int proc_id, struct sockaddr_in recipient);
