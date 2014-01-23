#ifndef rpsNetwork_h
#define rpsNetwork_h

class rpsTCPSocket;
class rpsCommunicator;
void rpsInitializeNetwork();
void rpsFinalizeNetwork();
bool rpsRunTCPServer(uint16_t port, const std::function<bool (rpsTCPSocket &client)> &handler);

class rpsTCPSocket
{
public:
    rpsTCPSocket();
    rpsTCPSocket(SOCKET s, bool needs_shutdown=true);
    rpsTCPSocket(const char *host, uint16_t port);
    ~rpsTCPSocket();
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
    rpsTCPSocket(const rpsTCPSocket &);
    rpsTCPSocket& operator=(const rpsTCPSocket &);
};

class rpsProtocolSocket
{
public:
    rpsProtocolSocket();
    rpsProtocolSocket(SOCKET s, bool needs_shutdown=true);
    rpsProtocolSocket(const char *host, uint16_t port);
    ~rpsProtocolSocket();
    bool open(const char *host, uint16_t port);
    void close();
    bool read(std::string &o_str);
    bool write(const void *data, uint32_t data_size);
private:
    rpsTCPSocket m_socket;
};

#endif // rpsNetwork_h
