#ifndef DISPATCHER_H_INCLUDED
#define DISPATCHER_H_INCLUDED

#include <map>

typedef char* (*callback)(class Connection *connection, char *id, char *data, size_t dataLength);

struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
        return std::strcmp(a, b) < 0;
   }
};

class Dispatcher
{
    public:
        Dispatcher() {};
        ~Dispatcher() {};

        void Register(char *call, callback callback);
        void Dispatch(class Connection *connection, char *call, char *id, char *data, size_t dataLength);
    
    private:
        std::map<char*, callback, cmp_str> callbacks;
};

#endif