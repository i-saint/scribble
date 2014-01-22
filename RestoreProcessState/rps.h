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
#ifdef max
#   undef max
#   undef min
#endif // max

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
};
typedef rpsIModule* (*rpsModuleCreator)();

typedef std::basic_string<char, std::char_traits<char>, rps_allocator<char> > rps_string;

class rpsProcessState
{
public:
    typedef std::map<rps_string, rpsIModule*, std::less<rps_string>, rps_allocator<std::pair<rps_string, rpsIModule*> > > Modules;
    typedef std::vector<rpsHookInfo*, rps_allocator<rpsHookInfo*> > Hooks;
    typedef std::map<rps_string, Hooks, std::less<rps_string>, rps_allocator<std::pair<rps_string, Hooks> > > HookTable;
    typedef std::map<rps_string, HookTable, std::less<rps_string>, rps_allocator<std::pair<rps_string, HookTable> > > DllHookTable;

    static void initialize();
    static rpsProcessState* getInstance();
    rpsProcessState();
    ~rpsProcessState();
    void serialize(rpsArchive &ar);
    void serialize(const char *path, rpsArchive::Mode mode);

    // F: [](rpsIModule*) -> void
    template<class F>
    void eachModules(const F &f)
    {
        for(auto i=m_modules.begin(); i!=m_modules.end(); ++i) {
            f(i->second);
        }
    }

private:
    void setHooks();

    Modules m_modules;
    DllHookTable m_hooks;
};

#endif // rps_h
