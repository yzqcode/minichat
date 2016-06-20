#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

#include <iostream>
#include <list>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
using namespace std;

// server ip
#define SERVER_IP "127.0.0.1"

// server port
#define SERVER_PORT 8888

//max epoll size
#define EPOLL_SIZE  5000

#endif // STDAFX_H_INCLUDED
