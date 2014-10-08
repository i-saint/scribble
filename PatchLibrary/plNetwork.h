#ifndef plNetwork_h
#define plNetwork_h

class plTCPSocket;
class plCommunicator;
void plInitializeNetwork();
void plFinalizeNetwork();
bool plRunTCPServer(uint16_t port, const std::function<bool (plTCPSocket &client)> &handler);

class plTCPSocket
{
public:
    plTCPSocket();
    plTCPSocket(SOCKET s, bool needs_shutdown=true);
    plTCPSocket(const char *host, uint16_t port);
    ~plTCPSocket();
    SOCKET getHandle() const;
    bool open(const char *host, uint16_t port);
    void close();
    int read(void *buf, uint32_t buf_size);
    int write(const void *data, uint32_t data_size);

private:
    SOCKET m_socket;
    bool m_needs_shutdown;

private:
    // forbidden
    plTCPSocket(const plTCPSocket &);
    plTCPSocket& operator=(const plTCPSocket &);
};

class plProtocolSocket
{
public:
    plProtocolSocket();
    plProtocolSocket(SOCKET s, bool needs_shutdown=true);
    plProtocolSocket(const char *host, uint16_t port);
    ~plProtocolSocket();
    bool open(const char *host, uint16_t port);
    void close();
    bool read(plString &o_str);
    bool write(const void *data, uint32_t data_size);
private:
    plTCPSocket m_socket;
};

#endif // rpsNetwork_h
