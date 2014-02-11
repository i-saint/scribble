#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsD3D9.h"

namespace {

class rpsD3D9Module : public rpsIModule
{
public:
    static rpsD3D9Module* getInstance();

    rpsD3D9Module();
    ~rpsD3D9Module();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);

private:
};
typedef rpsD3D9Module rpsCurrentModule;
inline rpsCurrentModule* rpsGetCurrentModule() { return rpsCurrentModule::getInstance(); }


Direct3DCreate9T    vaDirect3DCreate9;

rpsHookAPI IDirect3D9* WINAPI rpsDirect3DCreate9(UINT SDKVersion)
{
    IDirect3D9 *d3d9 = vaDirect3DCreate9(SDKVersion);
    if(d3d9!=nullptr) {
        return new rpsDirect3D9(d3d9);
    }
    return d3d9;
}

rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo2("d3d9.dll", Direct3DCreate9, rpsE_IEATOverride),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};




const char*     rpsD3D9Module::getModuleName() const { return "rpsD3D9Module"; }
rpsHookInfo*    rpsD3D9Module::getHooks() const      { return g_hookinfo; }

rpsD3D9Module* rpsD3D9Module::getInstance()
{
    static rpsD3D9Module *s_inst = new rpsD3D9Module();
    return s_inst;
}

rpsD3D9Module::rpsD3D9Module()
{
}

rpsD3D9Module::~rpsD3D9Module()
{
}

void rpsD3D9Module::initialize()
{
}

void rpsD3D9Module::serialize(rpsArchive &ar)
{
}


} // namespace

rpsDLLExport rpsIModule* rpsCreateD3D9Module() { return rpsD3D9Module::getInstance(); }
