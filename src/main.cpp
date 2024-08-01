#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

void startWinSock()
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
}

int mainShowIP(int argc, char** argv)
{
    addrinfo* addrInfos;
    int status;

    addrinfo hints{.ai_flags = argc == 0 ? AI_PASSIVE : 0, 
                   .ai_family = AF_UNSPEC, 
                   .ai_socktype = SOCK_DGRAM};

    if((status = getaddrinfo(argc == 0 ? nullptr : argv[1], nullptr, &hints, &addrInfos)) != 0)
    {
        fprintf(stderr, "Couldn't get address info : %s\n", gai_strerror(status));
    }
    else
    {
        for(addrinfo* addrInfo = addrInfos; addrInfo != nullptr; addrInfo = addrInfo->ai_next)
        {
            if(addrInfo->ai_family == AF_INET)
            {
                sockaddr_in* ipv4 = (sockaddr_in*)(addrInfo->ai_addr);
                char ipv4str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(ipv4->sin_addr), ipv4str, sizeof(ipv4str));
                printf("IPv4 : %s\n", ipv4str);
            }
            else if(addrInfo->ai_family == AF_INET6)
            {
                sockaddr_in6* ipv6 = (sockaddr_in6*)(addrInfo->ai_addr);
                char ipv6str[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6str, sizeof(ipv6str));
                printf("IPv6 : %s\n", ipv6str);
            }
        }
    }

    freeaddrinfo(addrInfos);

    WSACleanup();

    return 0;
}

int mainConnectSocket(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Please give one argument, the host name to connect to.");
        exit(1);
    }

    startWinSock();

    int status;
    addrinfo hints{};

    addrinfo* connectAddr;

    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(argv[1], "http", &hints, &connectAddr);
    if(status != 0) 
    {
        printf("Failed to get connection address info : %s\n", gai_strerror(status));
        WSACleanup();
        exit(1);
    }
    
    int socketFD = socket(connectAddr->ai_family, connectAddr->ai_socktype, connectAddr->ai_protocol);
    if(socketFD == -1)
    {
        printf("Failed to create socket. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(connectAddr);
        WSACleanup();
        exit(1);
    }

    status = connect(socketFD, connectAddr->ai_addr, connectAddr->ai_addrlen);
    if(status != 0)
    {
        printf("Failed to connect socket. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(connectAddr);
        WSACleanup();
        exit(1);
    }

    if(connectAddr->ai_family == AF_INET)
    {  
        sockaddr_in* ipv4 = (sockaddr_in*)(connectAddr->ai_addr);
        char myIP[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &(ipv4->sin_addr), myIP, sizeof(myIP)); 
        printf("Socket connected to address : port %hu on %s\n", ntohs(ipv4->sin_port), myIP);
    }
    else if(connectAddr->ai_family == AF_INET6)
    {
        sockaddr_in6* ipv6 = (sockaddr_in6*)(connectAddr->ai_addr);
        char myIP[INET6_ADDRSTRLEN];

        inet_ntop(AF_INET6, &(ipv6->sin6_addr), myIP, sizeof(myIP)); 
        printf("Socket connected to address : port %hu on %s\n", ntohs(ipv6->sin6_port), myIP);
    }

    freeaddrinfo(connectAddr);
    closesocket(socketFD);
    WSACleanup();

    return 0;
}