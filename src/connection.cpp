#include <Winsock2.h>
#include <stdio.h>
#include <thread>

#include "connection.h"
#include "common.h"
#include "dispatcher.h"
#include "server.h"

void Connection::Send(char *call, char *id, char *data, size_t dataLength)
{
    char *first = Format("%s-%s-", call, id);
    size_t firstLength = strlen(first);

    size_t messageLength = firstLength + dataLength;
    char *message = new char[messageLength];

    memcpy(message, first, firstLength);
    if (dataLength > 0) memcpy(message + firstLength, data, dataLength);

    send(socket, message, messageLength, 0);

    if (strcmp(call, "resping") != 0) printf("Sent to %d, %s: %.*s | %dB\n", playerIndex, address, firstLength - 1, first, dataLength);
}

void Connection::Listen()
{
    while (true)
    {
        char buffer[512];
        int length = recv(socket, buffer, 512, 0);
        if (length <= 0) break;

        char call[20];
        char id[20];
        char dataBuffer[512];
        char *data = 0;
        char *current = call;
        size_t j = 0;

        for (size_t i = 0; i < length; i++)
        {
            if (buffer[i] == '-' && current != dataBuffer)
            {
                if (current == call)
                {
                    current[i] = 0;
                    current = id;
                }
                else if (current == id)
                {
                    current[i - j] = 0;
                    current = dataBuffer;
                }

                j = i + 1;
                continue;
            }

            current[i - j] = buffer[i];
        }

        size_t dataLength = length - j;
        if (dataLength > 0)
        {
            data = new char[dataLength];
            memcpy(data, dataBuffer, dataLength);
        }

        if (strcmp(call, "reqping") != 0) printf("Received from %d, %s: %s-%s | %dB\n", playerIndex, address, call, id, dataLength);

        server->GetDispatcher().Dispatch(this, call, id, data, dataLength);
    }

    closesocket(socket);
    printf("%s disconnected\n", address);
    server->Disconnect(this);
}

Connection::Connection(Server *server, SOCKET socket, char* address) : server(server), socket(socket)
{
    this->address = address;
    for (size_t i = 0; i < 16; i++)
    {
        char *addr = address + i;
        if (*addr == 0) break;
        printf("%c", *addr);
    }
    printf(" connected\n");
    std::thread(&Connection::Listen, this).detach();
}