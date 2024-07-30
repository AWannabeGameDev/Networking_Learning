#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        WSACleanup();
        exit(1);
    }

    if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2))
    {
        fprintf(stderr, "Version 2.2 of Winsock is not available.\n");
        WSACleanup();
        exit(1);
    }

    addrinfo* myInfo;
    int status;

    addrinfo hints{};
    //hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(argv[1], nullptr, &hints, &myInfo)) != 0)
    {
        fprintf(stderr, "Couldn't get address info : \n", gai_strerror(status));
    }
    else
    {
        if(myInfo->ai_family == AF_INET)
        {
            sockaddr_in* ipv4 = (sockaddr_in*)(myInfo->ai_addr);
            char ipv4str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipv4str, sizeof(ipv4str));
            printf("My address : %s\n", ipv4str);
        }
        else if(myInfo->ai_family == AF_INET6)
        {
            sockaddr_in6* ipv6 = (sockaddr_in6*)(myInfo->ai_addr);
            char ipv6str[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6str, sizeof(ipv6str));
            printf("My address : %s\n", ipv6str);
        }
    }

    freeaddrinfo(myInfo);

    WSACleanup();
}