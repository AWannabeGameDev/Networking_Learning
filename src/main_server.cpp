#define WIN32_LEAN_AND_MEAN

#include "util.h"

int main()
{
    int status;

    status = startWSA();
    if(status != 0)
    {
        return 1;
    }

    addrinfo* myAddr;
    addrinfo hints
    {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM
    };

    status = getaddrinfo(nullptr, "3490", &hints, &myAddr);
    if(status != 0)
    {
        printf("Couldn't get local address info : %s\n", gai_strerror(status));
        WSACleanup();
        return 1;
    }

    int listenSocket = socket(myAddr->ai_family, myAddr->ai_socktype, myAddr->ai_protocol);
    if(listenSocket == -1)
    {
        printf("Couldn't create socket. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(myAddr);
        WSACleanup();
        return 1;
    }

    status = bind(listenSocket, myAddr->ai_addr, myAddr->ai_addrlen);
    if(status == -1)
    {
        printf("Couldn't bind socket. Error code : %d\n", WSAGetLastError());  
        freeaddrinfo(myAddr);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    IPandPort myIPandPort = getIPandPort(myAddr->ai_addr, myAddr->ai_family);
    printf("Ready to listen on port %hu on local IP %s\n", myIPandPort.port, myIPandPort.ip.data());

    int clientCommSocket;
    sockaddr_storage clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    listen(listenSocket, 10);
    clientCommSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);
    if(clientCommSocket == -1)
    {
        printf("Error accepting incoming request. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(myAddr);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    IPandPort clientIPandPort = getIPandPort((sockaddr*)&clientAddr, clientAddr.ss_family);
    printf("Accepted connection from port %hu on host %s\n", clientIPandPort.port, clientIPandPort.ip.data());

    const char* msg = "This is the first server written by Nandan the GOAT!";
    status = send(clientCommSocket, msg, strlen(msg), 0);
    if(status == -1)
    {
        printf("Error sending message to client. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(myAddr);
        closesocket(clientCommSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    char recvedMsg[256];
    status = recv(clientCommSocket, recvedMsg, sizeof(recvedMsg), 0);
    if(status == -1)
    {
        printf("Error receiving message from client. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(myAddr);
        closesocket(clientCommSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Message from client : %s\n", recvedMsg);

    freeaddrinfo(myAddr);
    closesocket(clientCommSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}