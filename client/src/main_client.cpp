#define WIN32_LEAN_AND_MEAN

#include <util.h>

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        printf("Please give two arguments, the server IP and port");
        return 1;
    }

    int status;

    status = startWSA();
    if(status != 0)
    {
        return 1;
    }

    addrinfo* serverAddr;
    addrinfo hints
    {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };

    status = getaddrinfo(argv[1], argv[2], &hints, &serverAddr);
    if(status != 0)
    {
        printf("Couldn't get server address info : %s\n", gai_strerror(status));
        WSACleanup();
        return 1;
    }

    int serverSocket = socket(serverAddr->ai_family, serverAddr->ai_socktype, serverAddr->ai_protocol);
    if(serverSocket == -1)
    {
        printf("Couldn't create socket. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(serverAddr);
        WSACleanup();
        return 1;
    }

    status = connect(serverSocket, serverAddr->ai_addr, serverAddr->ai_addrlen);
    if(status == -1)
    {
        printf("Failed to connect to server. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(serverAddr);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    IPandPort serverIPandPort = getIPandPort(serverAddr->ai_addr, serverAddr->ai_family);
    printf("Connected to server on port %hu on IP %s\n", serverIPandPort.port, serverIPandPort.ip.data());

    const char* msg = "This is the first client written by Nandan the GOAT!";
    status = send(serverSocket, msg, strlen(msg), 0);
    if(status == -1)
    {
        printf("Failed to send message to server. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(serverAddr);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    char recvedMsg[256];
    status = recv(serverSocket, recvedMsg, sizeof(recvedMsg), 0);
    if(status == -1)
    {
        printf("Failed to receive message from server. Error code : %d\n", WSAGetLastError());
        freeaddrinfo(serverAddr);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Message from server : %s\n", recvedMsg);

    freeaddrinfo(serverAddr);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}