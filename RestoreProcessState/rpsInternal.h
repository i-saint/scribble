#ifndef rpsInternal_h
#define rpsInternal_h

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#include <tlhelp32.h>
#ifdef max
#   undef max
#   undef min
#endif // max

#include "rps.h"
#include "rpsMalloc.h"
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")


#define rpsDLLExport     __declspec(dllexport)
#define rpsDLLImport     __declspec(dllimport)


class rpsArchive
{
public:
	enum Mode
	{
		Unknown,
		Reader,
		Writer,
	};

	rpsArchive();
	~rpsArchive();
	void read(void *dst, size_t size);
	void write(const void *data, size_t size);
	void io(void *dst, size_t size);
	bool open(const char *path_to_file, Mode mode);
	void close();
	bool isReader() const { return m_mode==Reader; }
	bool isWriter() const { return m_mode==Writer; }

private:
	FILE *m_file;
	Mode m_mode;
};


struct rpsHookInfo
{
	const char *dllname;
	const char *funcname;
	uint32_t funcordinal;
	void *hookfunc;
	void **origfunc;
	uint32_t nth;

	rpsHookInfo(const char *dll=nullptr, const char *fname=nullptr, uint32_t ford=0, void *hook=nullptr, void **orig=nullptr)
		: dllname(dll)
		, funcname(fname)
		, funcordinal(ford)
		, hookfunc(hook)
		, origfunc(orig)
		, nth(0)
	{
	}
};


class rpsIModule
{
public:
	virtual ~rpsIModule() {}
	virtual void			release() { delete this; }
	virtual const char*		getModuleName() const=0;
	virtual size_t			getNumHooks() const=0;
	virtual rpsHookInfo*	getHooks() const=0;
	virtual void			serialize(rpsArchive &ar)=0;

	void* operator new(size_t s) { return rpsMalloc(s); }
	void  operator delete(void *p) { return rpsFree(p); }
};
typedef rpsIModule* (*rpsModuleCreator)();

typedef std::basic_string<char, std::char_traits<char>, rps_allocator<char> > rps_string;

class rpsProcessState
{
public:
	static void initialize();
	static void serialize(const char *path, rpsArchive::Mode mode);

	void* operator new(size_t s) { return rpsMalloc(s); }
	void  operator delete(void *p) { return rpsFree(p); }

private:
	friend DWORD __stdcall rpsMainThread(LPVOID lpThreadParameter);

	struct SerializeRequest
	{
		rps_string path;
		rpsArchive::Mode mode;
	};

	typedef std::map<rps_string, rpsIModule*, std::less<rps_string>, rps_allocator<std::pair<rps_string, rpsIModule*> > > Modules;
	typedef std::vector<rpsHookInfo*, rps_allocator<rpsHookInfo*> > Hooks;
	typedef std::map<rps_string, Hooks, std::less<rps_string>, rps_allocator<std::pair<rps_string, Hooks> > > FuncHookTable;
	typedef std::map<rps_string, FuncHookTable, std::less<rps_string>, rps_allocator<std::pair<rps_string, FuncHookTable> > > DLLHookTable;
	typedef std::vector<SerializeRequest, rps_allocator<SerializeRequest> > Requests;

	static rpsProcessState* getInstance();
	rpsProcessState();
	~rpsProcessState();
	void mainloop();
	void serializeImpl(rpsArchive &ar);
	void serializeImpl(const char *path, rpsArchive::Mode mode);

	// F: [](rpsIModule*) -> void
	template<class F>
	void eachModules(const F &f)
	{
		for(auto i=m_modules.begin(); i!=m_modules.end(); ++i) {
			f(i->second);
		}
	}

	Modules m_modules;
	DLLHookTable m_hooks;
	Requests m_requests;
};

#include "rpsInlines.h"

#endif // rpsInternal_h
