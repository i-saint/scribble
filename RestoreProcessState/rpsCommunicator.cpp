#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsNetwork.h"
#include "rpsCommunicator.h"

rpsCommunicator::rpsCommunicator()
    : m_running(false)
    , m_port(0)
{
}

rpsCommunicator::~rpsCommunicator()
{
    stop();
}

bool rpsCommunicator::run(uint16_t port)
{
    if(m_running) { return false; }

    m_port = port;
    m_running = true;
    rpsRunThread([&](){
        {
            rpsMessage mes("rpsThreadModule", "addExcludeThread", ::GetCurrentThreadId());
            rpsSendMessage(mes);
        }

        bool r = rpsRunTCPServer(m_port, [&](rpsTCPSocket &client){
            rpsProtocolSocket s(client.getHandle(), false);
            return onAccept(s);
        });
        m_running = false;
    });
    return true;
}

void rpsCommunicator::stop()
{
    if(!m_running) { return; }
    // todo
}

bool rpsCommunicator::onAccept(rpsProtocolSocket &client)
{
    rps_string command;
    client.read(command);
    if(strncmp(&command[0], "save ", 5)==0) {
        rpsMainModule::SerializeRequest req = {rps_string(&command[5]), rpsArchive::Writer};
        rpsMainModule::getInstance()->pushRequest(req);
    }
    else if(strncmp(&command[0], "load ", 5)==0) {
        rpsMainModule::SerializeRequest req = {rps_string(&command[5]), rpsArchive::Reader};
        rpsMainModule::getInstance()->pushRequest(req);
    }
    else if(strncmp(&command[0], "stop", 4)==0) {
        return false;
    }
    return true;
}
