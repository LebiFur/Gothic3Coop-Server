#include "dispatcher.h"

void Dispatcher::Register(char *call, callback callback)
{
    callbacks[call] = callback;
}

void Dispatcher::Dispatch(class Connection *connection, char *call, char *id, char *data, size_t dataLength)
{
    if (!callbacks.count(call))
    {
        printf("Dispatch call %s not found\n", call);
        return;
    }
    char *message = callbacks[call](connection, id, data, dataLength);
}