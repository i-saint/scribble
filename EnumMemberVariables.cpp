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
};

std::string GetSymbolName(const EMVContext &ctx, DWORD t)
{
    WCHAR *wname = NULL;
    if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMNAME, &wname )) {
        char name[1024];
        size_t num = 0;
        ::wcstombs_s(&num, name, wname, _countof(name));
        ::LocalFree(wname);
        return name;
    }
    return "";
}

template<class Func>
void EnumMemberVariables(EMVContext &ctx, DWORD t, const Func &f)
{
    DWORD tag;
    ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMTAG, &tag );

    if(tag==SymTagData) {
        std::string name = ctx.current_class;
        name += "::";
        name += GetSymbolName(ctx, t);
        f(name);
    }
    else if(tag==SymTagBaseClass) {
        DWORD type;
        if( ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPE, &type ) ) {
            EnumMemberVariables<Func>(ctx, type, f);
        }
    }
    else if(tag==SymTagUDT) {
        std::string prev = ctx.current_class;
        ctx.current_class = GetSymbolName(ctx, t);

        DWORD num_members = 0;
        if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_CHILDRENCOUNT, &num_members )) {
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

// Func: (std::string &name_of_member_variable)
template<class Func>
bool EnumMemberVariables(const char *classname, const Func &f)
{
    char buf[sizeof(SYMBOL_INFO)+MAX_PATH];
    PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
    sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    sinfo->MaxNameLen = MAX_PATH;

    EMVContext ctx = {::GetCurrentProcess(), 0};
    {
        HMODULE mod = 0;
        void *addr = *(void**)_AddressOfReturnAddress();
        ::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)addr, &mod);
        ctx.modbase = (ULONG64)mod;
    }

    if(::SymGetTypeFromName(ctx.hprocess, ctx.modbase, classname, sinfo)) {
        EnumMemberVariables(ctx, sinfo->TypeIndex, f);
        return true;
    }
    return false;
}




#define USG_Serializable(ClassName)\
    void serialize();\
    void deserialize();

#define USG_ImplementSerializer(ClassName)\
    void ClassName::Serialize()\
    {\
        EnumMemberVariables(#ClassName, [](std::string &name){\
            printf("%s\n", name.c_str());\
        });\
    }\
    void ClassName::Deserialize()\
    {\
    }


class Hoge
{
public:
    int a;
    int b;
};

class Hage : public Hoge
{
public:
    int c;
    int d;

    USG_Serializable(Hage)
};

USG_ImplementSerialize(Hage)

void Hage::Serialize()
{
}


int main(int argc, char* argv[])
{
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    EnumMemberVariables("Hage", [](std::string &name){
        printf("%s\n", name.c_str());
    });

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
