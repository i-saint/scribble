#ifndef rpsCommunicator_h
#define rpsCommunicator_h

class rpsCommunicator
{
public:
    rpsCommunicator();
    ~rpsCommunicator();
    bool run(uint16_t port, bool auto_flush);
    void stop();

    void* operator new(size_t s) { return rpsMalloc(s); }
    void  operator delete(void *p) { return rpsFree(p); }

private:
    bool onAccept(rpsProtocolSocket &client);

    bool        m_running;
    bool        m_auto_flush;
    uint16_t    m_port;
};

#endif // rpsCommunicator_h
