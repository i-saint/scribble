#include "rps.h"
#include "rpsInlines.h"

class rpsFiles : public rpsIModule
{
public:
	static rpsFiles* getInstance();

	rpsFiles();
	~rpsFiles();
	virtual const char*		getModuleName() const;
	virtual size_t			getNumHooks() const;
	virtual rpsHookInfo*	getHooks() const;
	virtual void serialize(rpsArchive &ar);

	HANDLE translate(HANDLE h);

private:
};


typedef HANDLE (WINAPI *CreateFileT)(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
	);

CreateFileT origCreateFile;

HANDLE WINAPI rpsCreateFile(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
	)
{
	HANDLE ret = origCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	return ret;
}

static rpsHookInfo g_rps_hooks[] = {
	{"kernel32.dll", "CreateFile", 0, rpsCreateFile, &(void*&)origCreateFile},
};


const char*		rpsFiles::getModuleName() const	{ return "rpsFiles"; }
size_t			rpsFiles::getNumHooks() const	{ return _countof(g_rps_hooks); }
rpsHookInfo*	rpsFiles::getHooks() const		{ return g_rps_hooks; }


rpsFiles* rpsFiles::getInstance()
{
	static rpsFiles *s_inst;
	if(!s_inst) { s_inst = new rpsFiles(); }
	return s_inst;
}

rpsFiles::rpsFiles()
{
}

rpsFiles::~rpsFiles()
{
}

void rpsFiles::serialize(rpsArchive &ar)
{
}

HANDLE rpsFiles::translate(HANDLE h)
{
	return nullptr;
}

rpsIModule* rpsCreateFiles() { return rpsFiles::getInstance(); }
