#include "util.h"

int startWSA()
{
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Failed to start winsock.\n");
        return 1;
    }

    if((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2))
    {
        printf("Requested version of winsock (2.2) isn't available.\n");
        WSACleanup();
        return 1;
    }

    return 0;
}

IPandPort getIPandPort(sockaddr* addr, int family)
{
    if(family == AF_INET)
    {
        const sockaddr_in* ipv4 = (sockaddr_in*)addr;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, ipv4, ip, sizeof(ip));

        unsigned short port = ntohs(ipv4->sin_port);

        return IPandPort{.ip = ip, .port = port};
    }
    else if(family == AF_INET6)
    {
        const sockaddr_in6* ipv6 = (sockaddr_in6*)addr;

        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, ipv6, ip, sizeof(ip));

        unsigned short port = ntohs(ipv6->sin6_port);

        return IPandPort{.ip = ip, .port = port};
    }
    else
    {
        return {};
    }
}