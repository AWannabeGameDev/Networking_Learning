#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

struct IPandPort
{
    std::string ip;
    unsigned short port;
};

int startWSA();
IPandPort getIPandPort(sockaddr* addr, int family);