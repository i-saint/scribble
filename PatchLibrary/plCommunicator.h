#ifndef plCommunicator_h
#define plCommunicator_h

#include "plInternal.h"
#include "plNetwork.h"

class plCommunicator
{
public:
    plCommunicator();
    ~plCommunicator();
    bool run(uint16_t port);
    void stop();

private:
    bool onAccept(plProtocolSocket &client);

    bool        m_running;
    uint16_t    m_port;
};

#endif // plCommunicator_h
