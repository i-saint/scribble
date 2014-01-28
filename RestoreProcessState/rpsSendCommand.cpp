#include "rpsInternal.h"
#include "rpsNetwork.h"

const char* rpsGetMainModuleFilename()
{
    static char s_path[MAX_PATH+1];
    static char *s_filename = nullptr;
    if(!s_filename) {
        ::GetModuleFileNameA(nullptr, s_path, sizeof(s_path));
        s_filename = s_path;
        for(int i=0; ; ++i) {
            if(s_path[i]=='\\') { s_filename=s_path+i+1; }
            else if(s_path[i]=='\0') { break; }
        }
    }
    return s_filename;
}


int main(int argc, char *argv[])
{
    if(argc<4) {
        printf("usage %s [hostname(:port)] (save|load|timescale|stop) params\n", rpsGetMainModuleFilename());
        return 0;
    }

    rpsInitializeFoundation();
    rpsInitializeNetwork();

    const char *host = argv[1];
    uint16_t port = rpsDefaultPort;
    rps_string command;
    for(int i=2; i<argc; ++i) {
        if(!command.empty()) { command+=" "; }
        command += argv[i];
    }

    rpsProtocolSocket sock;
    if(sock.open(host, port)) {
        sock.write(command.c_str(), command.size());
    }
}
