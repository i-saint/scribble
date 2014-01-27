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
		printf("usage %s [hostname(:port)] (save|load) path-to-state-file\n", rpsGetMainModuleFilename());
		return 0;
	}

	rpsInitializeFoundation();
	rpsInitializeNetwork();

	const char *host = argv[1];
	uint16_t port = rpsDefaultPort;
	rps_string command;
	if(strcmp(argv[2], "save")==0) {
		command += "save ";
	}
	else {
		command += "load ";
	}
	command += argv[3];

	rpsProtocolSocket sock;
	if(sock.open(host, port)) {
		sock.write(command.c_str(), command.size());
	}
}
