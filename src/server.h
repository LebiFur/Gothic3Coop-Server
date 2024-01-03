#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <WinSock2.h>
#include <vector>

class Server
{
    public:
        Server(class Dispatcher &dispatcher) : dispatcher(dispatcher) {};
        ~Server() {};

        std::vector<class Connection*> connections;

        bool Start(int port);
        class Dispatcher& GetDispatcher() { return dispatcher; }
        void Disconnect(class Connection*);
        void Exit();
        uint16_t NextIndex();
    
    private:
        int port;
        SOCKET listenSocket;
        class Dispatcher &dispatcher;
        uint16_t currentIndex = 1;

        void Listen();
        void Respond();
};

#endif