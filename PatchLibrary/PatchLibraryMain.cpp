#include "plCommunicator.h"

plCommunicator *g_communicator;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		plInitializeNetwork();
		g_communicator = new plCommunicator();
		g_communicator->run(plDefaultPort);
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {
	}
	return TRUE;
}
