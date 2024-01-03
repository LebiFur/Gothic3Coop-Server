#include <Winsock2.h>
#include <stdio.h>
#include <thread>

#include "common.h"
#include "server.h"
#include "dispatcher.h"
#include "connection.h"

Server *server;

char* Ping(Connection *connection, char *id, char *data, size_t dataLength)
{
    connection->Send("resping", id, data, dataLength);
    return 0;
}

char* Start(Connection *connection, char *id, char *data, size_t dataLength)
{
    char playersBuffer[512];

    uint16_t index = server->NextIndex();
    memcpy(playersBuffer, &index, sizeof(uint16_t));

    size_t offset = sizeof(uint16_t);

    for (Connection *con : server->connections)
    {
        if (con->playerIndex == 0) continue;

        char nameLength = strlen(con->playerName) + 1;

        memcpy(playersBuffer + offset, &con->playerIndex, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        memcpy(playersBuffer + offset, &nameLength, sizeof(char));
        offset += sizeof(char);

        memcpy(playersBuffer + offset, con->playerName, nameLength);
        offset += nameLength;
    }
    
    connection->Send("resstart", id, playersBuffer, offset);

    connection->playerIndex = index;
    connection->playerName = new char[dataLength];
    strcpy(connection->playerName, data);

    printf("New player: %s | %d\n", data, index);

    char nameLength = strlen(connection->playerName) + 1;
    const size_t bufferSize = sizeof(uint16_t) + sizeof(char) + nameLength;
    char *buffer = new char[bufferSize];

    memcpy(buffer, &connection->playerIndex, sizeof(uint16_t));
    memcpy(buffer + sizeof(uint16_t), &nameLength, sizeof(char));
    memcpy(buffer + sizeof(uint16_t) + sizeof(char), connection->playerName, nameLength);

    for (Connection *con : server->connections)
    {
        if (con->playerIndex == index) continue;

        con->Send("connect", "0", buffer, bufferSize);
    }

    return 0;
}

char* Update(Connection *connection, char *id, char *data, size_t dataLength)
{
    for (Connection *con : server->connections)
    {
        if (con->playerIndex == 0 || con->playerIndex == connection->playerIndex) continue;

        const size_t bufferSize = sizeof(uint16_t) + 64;
        char buffer[bufferSize];
        memcpy(buffer, &connection->playerIndex, sizeof(uint16_t));
        memcpy(buffer + sizeof(uint16_t), data, 64);

        con->Send("update", id, buffer, bufferSize);
    }

    return 0;
}

int main()
{
    Dispatcher dispatcher = Dispatcher();
    dispatcher.Register("reqping", Ping);
    dispatcher.Register("reqstart", Start);
    dispatcher.Register("update", Update);

    server = new Server(dispatcher);
    if (!server->Start(21370)) return 1;

    while (true)
    {
        char command[20];
        scanf("%19s", command);

        if (strcmp(command, "exit") == 0) break;
        else if (strcmp(command, "clear") == 0) system("cls");
        else
        {
            printf("Available commands:\n");
            printf("clear - clears the output\n");
            printf("exit - exits the server\n");
        }
    }

    server->Exit();

    return 0;
}