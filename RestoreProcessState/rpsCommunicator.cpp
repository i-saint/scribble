#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsNetwork.h"
#include "rpsCommunicator.h"

rpsCommunicator::rpsCommunicator()
    : m_running(false)
    , m_auto_flush(false)
    , m_port(0)
{
}

rpsCommunicator::~rpsCommunicator()
{
    stop();
}

bool rpsCommunicator::run(uint16_t port, bool auto_flush)
{
    if(m_running) { return false; }

    m_auto_flush = auto_flush;
    m_port = port;
    m_running = true;
    rpsRunThread([&](){
        bool r = rpsRunTCPServer(m_port, [&](rpsTCPSocket &client){
            rpsProtocolSocket s(client.getHandle(), false);
            return onAccept(s);
        });
        if(r) {
            rpsPrintInfo("communicator started. port: %d\n", (int)port);
        }
        m_running = false;
    });
    return true;
}

void rpsCommunicator::stop()
{
    if(!m_running) { return; }
}

bool rpsCommunicator::onAccept(rpsProtocolSocket &client)
{
    return true;
}
