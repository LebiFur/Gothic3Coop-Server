#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include <Winsock2.h>
#include <thread>

class Connection
{
    public:
        Connection(class Server *server, SOCKET socket, char *address);
        ~Connection() {};

        uint16_t playerIndex = 0;
        char* playerName;

        void Listen();
        void Send(char *call, char *id, char *data, size_t dataLength);
    
    private:
        class Server *server;
        SOCKET socket;
        char *address;
};

#endif