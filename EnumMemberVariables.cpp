#include <cstdio>
#include <string>

#define _NO_CVCONST_H
#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")


struct EMVContext
{
    HANDLE hprocess;
    ULONG64 modbase;
    std::string current_class;
    std::string current_type;
    std::string current_value;
    std::string tmp_name;
};

enum BasicType
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};

bool GetSymbolName(EMVContext &ctx, DWORD t)
{
    WCHAR *wname = NULL;
    if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMNAME, &wname )) {
        size_t num = 0;
        char out[MAX_SYM_NAME];
        ::wcstombs_s(&num, out, wname, _countof(out));
        ctx.tmp_name = out;
        ::LocalFree(wname);
        return true;
    }
    return false;
}

DWORD GetSymbolTypeID(EMVContext &ctx, DWORD t)
{
    DWORD tid = 0;
    if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPEID, &tid)) {
        return tid;
    }
    return 0;
}

bool GetSymbolTypeNameImpl(EMVContext &ctx, DWORD t, std::string &ret)
{
    DWORD tag;
    DWORD basetype = 0;
    ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMTAG, &tag);
    if(tag==SymTagArrayType) {
        DWORD count = 0;
        ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_COUNT, &count);
        char a[128];
        sprintf(a, "[%d]", count);
        ret += a;

        DWORD tid = GetSymbolTypeID(ctx, t);
        return GetSymbolTypeNameImpl(ctx, tid, ret);
    }
    else if(tag==SymTagPointerType) {
        ret = "*"+ret;

        DWORD tid = GetSymbolTypeID(ctx, t);
        return GetSymbolTypeNameImpl(ctx, tid, ret);
    }
    else if(tag==SymTagBaseType) {
        ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_BASETYPE, &basetype);
        ULONG64 length = 0;
        ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_LENGTH, &length);
        std::string type;
        switch(basetype) {
        case btChar:  type="char"; break;
        case btWChar: type="wchar"; break;
        case btBool:  type="bool"; break;
        case btInt:   type="int"; break;
        case btUInt:  type="uint"; break;
        case btFloat: type="float"; break;
        }
        switch(basetype) {
        case btInt:
        case btUInt:
        case btFloat:
            char bits[32];
            sprintf(bits, "%d", length*8);
            type+=bits;
            break;
        }
        ret = type+ret;
    }
    else {
        WCHAR *wname = nullptr;
        if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMNAME, &wname )) {
            char name[MAX_SYM_NAME];
            size_t num = 0;
            ::wcstombs_s(&num, name, wname, _countof(name));
            ::LocalFree(wname);
            ret+=name;
        }
    }
    return true;
}

bool GetSymbolTypeName(EMVContext &ctx, DWORD t)
{
    ctx.tmp_name.clear();
    return GetSymbolTypeNameImpl(ctx, t, ctx.tmp_name);
}

template<class Func>
void EnumMemberVariables(EMVContext &ctx, DWORD t, const Func &f)
{
    DWORD tag;
    ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMTAG, &tag );

    if(tag==SymTagData) {
        DWORD tid = GetSymbolTypeID(ctx, t);
        GetSymbolTypeName(ctx, tid);
        ctx.current_type = ctx.tmp_name;
        GetSymbolName(ctx, t);
        ctx.current_value = ctx.tmp_name;
        f(ctx.current_class, ctx.current_type, ctx.current_value);
    }
    else if(tag==SymTagBaseClass) {
        DWORD type;
        if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPE, &type)) {
            EnumMemberVariables<Func>(ctx, type, f);
        }
    }
    else if(tag==SymTagVTable) {
        // todo
    }
    else if(tag==SymTagUDT) {
        std::string prev = ctx.current_class;
        GetSymbolName(ctx, t);
        ctx.current_class = ctx.tmp_name;
        DWORD num_members = 0;
        if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_CHILDRENCOUNT, &num_members)) {
            TI_FINDCHILDREN_PARAMS *params = (TI_FINDCHILDREN_PARAMS*)malloc(sizeof(TI_FINDCHILDREN_PARAMS ) + (sizeof(ULONG)*num_members));
            params->Count = num_members;
            params->Start = 0;
            if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_FINDCHILDREN, params )) {
                for(DWORD i=0; i<num_members; ++i) {
                    EnumMemberVariables<Func>(ctx, params->ChildId[i], f);
                }
            }
            free(params);
        }
        ctx.current_class = prev;
    }
}

// Func: (const char *classname, const char *typename, const char *valuename)
template<class Func>
bool EnumMemberVariables(const char *classname, const Func &f)
{
    char buf[sizeof(SYMBOL_INFO)+MAX_PATH];
    memset(buf, 0, sizeof(buf));
    PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
    sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    sinfo->MaxNameLen = MAX_PATH;

    EMVContext ctx = {::GetCurrentProcess(), (ULONG64)::GetModuleHandleA(nullptr)};
    if(::SymGetTypeFromName(ctx.hprocess, ctx.modbase, classname, sinfo)) {
        EnumMemberVariables(ctx, sinfo->TypeIndex, f);
        return true;
    }
    return false;
}





class Hoge
{
public:
    virtual void test()=0;
    char            i1;
    unsigned char   i2;
    int             i3;
    unsigned int    i4;
    unsigned int    i5[4];
    unsigned int    *i6[4][8];
};

class Hage : public Hoge
{
public:
    virtual void test() {}
    float   f1;
    double  f2;
    bool    b;
    std::string e;
};

int main(int argc, char* argv[])
{
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);

    EnumMemberVariables("Hage", [](std::string &class_name, std::string &type_name, std::string &value_name){
        printf("%s %s::%s\n", type_name.c_str(), class_name.c_str(), value_name.c_str());
    });

    Hoge *h = new Hage();
    printf("%d\n", h->i1);

    return 0;
}

/*
> cl /EHsc /Zi EnumMemberVariables.cpp
> EnumMemberVariables

Hoge::a
Hoge::b
Hage::c
Hage::d
*/
