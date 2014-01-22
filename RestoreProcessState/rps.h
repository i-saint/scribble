#ifndef rps_h
#define rps_h

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
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")


#define rpsDLLExport     __declspec(dllexport)
#define rpsDLLImport     __declspec(dllimport)


class rpsArchive
{
public:
	rpsArchive();
	~rpsArchive();
	void read(void *dst, size_t size);
	void write(const void *data, size_t size);
	void io(void *dst, size_t size);
	bool open(const char *path_to_file, bool is_reader);
	void close();
	bool isReader() const { return m_is_reader; }

private:
	FILE *m_file;
	bool m_is_reader;
};
inline rpsArchive& operator&(rpsArchive &ar,     char v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,   int8_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  uint8_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  int16_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, uint16_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  int32_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, uint32_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  int64_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, uint64_t v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,    float v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,   double v) { ar.io(&v, sizeof(v)); return ar; }


struct rpsHookInfo
{
	const char *dllname;
	const char *funcname;
	uint32_t funcordinal;
	void *hookfunc;
	void **origfunc;
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
};
typedef rpsIModule* (*rpsModuleCreator)();


class rpsProcessState
{
public:
	typedef std::map<std::string, rpsIModule*> Modules;

	static rpsProcessState* getInstance();
	rpsProcessState();
	~rpsProcessState();
	void serialize(rpsArchive &ar);

	// F: [](rpsIModule*) -> void
	template<class F>
	void eachModules(const F &f)
	{
		for(auto i=m_modules.begin(); i!=m_modules.end(); ++i) {
			f(i->second);
		}
	}

private:
	Modules m_modules;
};

#endif // rps_h
