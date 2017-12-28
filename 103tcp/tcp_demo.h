#ifndef __TCP_DEMO_H
#define __TCP_DEMO_H
#include "types.h"

extern uint16 W5500_tcp_server_port;
void do_tcp_server(int fd);//TCP Server
void do_tcp_client(int fd);//TCP Clinet
#endif 

