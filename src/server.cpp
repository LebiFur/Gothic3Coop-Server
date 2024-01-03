#include <stdio.h>
#include <thread>
#include <vector>

#include "server.h"
#include "connection.h"
#include "common.h"

uint16_t Server::NextIndex()
{
    uint16_t playerIndex = currentIndex;
    currentIndex++;
    return playerIndex;
}

void Server::Disconnect(Connection *connection)
{
    connections.erase(std::remove(connections.begin(), connections.end(), connection));

    for (Connection *con : connections)
    {
        con->Send("disconnect", "0", reinterpret_cast<char*>(&connection->playerIndex), sizeof(uint16_t));
    }
    //printf("Disconnect called from %p\n", connection);
}

void Server::Listen()
{
    listen(listenSocket, 10);
    printf("Listening on port %d\n", port);

    while (true)
    {
        sockaddr_in clientAddress;
        int clientAddressLength = sizeof(clientAddress);

        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddress, &clientAddressLength);
        
        if (clientSocket == INVALID_SOCKET)
        {
            int error = WSAGetLastError();
            if (error == 10004) break;
            printf(Format("Connection failed with error: %d\n", error));
        }
        else
        {
            connections.emplace_back(new Connection(this, clientSocket, inet_ntoa(clientAddress.sin_addr)));
        }
    }
}

bool Server::Start(int port)
{
    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        printf("Error at WSAStartup with error: %d\n", iResult);
        return false;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("Socket failed with error: %u\n", WSAGetLastError());
        WSACleanup();
        return false;
    }

    struct sockaddr_in addr = {
        AF_INET,
        htons(port),
        0
    };

    iResult = bind(listenSocket, (SOCKADDR*)&addr, sizeof(addr));
    if (iResult == SOCKET_ERROR)
    {
        printf("Bind failed with error %u\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    this->port = port;

    std::thread(&Server::Listen, this).detach();

    return true;
}

void Server::Exit()
{
    closesocket(listenSocket);
    WSACleanup();
}