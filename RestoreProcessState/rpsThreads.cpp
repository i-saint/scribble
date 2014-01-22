#include "rps.h"
#include "rpsInlines.h"

class rpsThreads : public rpsIModule
{
public:
	static rpsThreads* getInstance();

	rpsThreads();
	~rpsThreads();
	virtual const char*		getModuleName() const;
	virtual size_t			getNumHooks() const;
	virtual rpsHookInfo*	getHooks() const;
	virtual void serialize(rpsArchive &ar);

	HANDLE translate(HANDLE h);

private:
};


typedef LPVOID (WINAPI *CreateThreadT)(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	DWORD dwCreationFlags,
	LPDWORD lpThreadId
	);

CreateThreadT origCreateThread;

LPVOID WINAPI rpsCreateThread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	DWORD dwCreationFlags,
	LPDWORD lpThreadId
	)
{
	LPVOID ret = origCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
	return ret;
}

static rpsHookInfo g_rps_hooks[] = {
	rpsHookInfo("kernel32.dll", "CreateThread",   0, rpsCreateThread, &(void*&)origCreateThread),
};


const char*		rpsThreads::getModuleName() const	{ return "rpsThreads"; }
size_t			rpsThreads::getNumHooks() const		{ return _countof(g_rps_hooks); }
rpsHookInfo*	rpsThreads::getHooks() const		{ return g_rps_hooks; }

rpsThreads* rpsThreads::getInstance()
{
	static rpsThreads *s_inst = nullptr;
	if(!s_inst) { s_inst = new rpsThreads(); }
	return s_inst;
}

rpsThreads::rpsThreads()
{
}

rpsThreads::~rpsThreads()
{
}

void rpsThreads::serialize(rpsArchive &ar)
{
}

HANDLE rpsThreads::translate(HANDLE h)
{
	return nullptr;
}

rpsIModule* rpsCreateThreads() { return rpsThreads::getInstance(); }
