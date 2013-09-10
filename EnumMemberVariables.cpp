#include <cstdio>
#include <string>
#include <functional>

#define _NO_CVCONST_H
#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")


struct MemberInfo
{
    void *this_pointer;
    const char *this_type;  // this の型
    const char *class_name; // メンバ変数が所属する class。親 class のメンバの場合 this_type とは違うものになる
    const char *type_name;  // メンバ変数の型名
    const char *value_name; // メンバ変数の名前
    void *data;             // メンバ変数へのポインタ
};
typedef std::function<void (const MemberInfo&)> MemberInfoCallback;

bool EnumMemberVariablesByTypeName(const char *classname, const MemberInfoCallback &f);
bool EnumMemberVariablesByPointer(void *_this, const MemberInfoCallback &f);


class Parent1
{
public:
    Parent1() : i1(1), i2(2), i3(3), i4(4)
    {}

    virtual int test()=0;
    char            i1;
    unsigned char   i2;
    int             i3;
    unsigned int    i4;
    unsigned int    i5[4];
    unsigned int    *i6[4][8];
};

class Parent2
{
public:
    Parent2() : f1(1.0f), f2(2.0)
    {}

    virtual int test2()=0;
    float           f1;
    double          f2;
};

class Child : public Parent1, public Parent2
{
public:
    int test()  override { return 1; }
    int test2() override { return 2; }
    bool    b;
    __m128  c;
    std::string str;
};

int main(int argc, char* argv[])
{
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);

    printf("EnumMemberVariablesByTypeName(\"Child\"):\n");
    EnumMemberVariablesByTypeName("Child", [](const MemberInfo &mi){
        printf("%s %s::%s\n", mi.type_name, mi.class_name, mi.value_name);
    });
    printf("\n");

    Parent1 *h = new Child();
    printf("EnumMemberVariablesByPointer(Child*):\n");
    EnumMemberVariablesByPointer(h, [](const MemberInfo &mi){
        printf("%s %s::%s\n", mi.type_name, mi.class_name, mi.value_name);
    });

    return 0;
}

/*
> cl /EHsc /Zi EnumMemberVariables.cpp
> EnumMemberVariables

EnumMemberVariablesByTypeName("Child"):
char Parent1::i1
uint8 Parent1::i2
int32 Parent1::i3
uint32 Parent1::i4
uint32[4] Parent1::i5
uint32*[4][8] Parent1::i6
float32 Parent2::f1
float64 Parent2::f2
bool Child::b
__m128 Child::c
std::basic_string<char,std::char_traits<char>,std::allocator<char> > Child::str

EnumMemberVariablesByPointer(Child*):
char Parent1::i1
uint8 Parent1::i2
int32 Parent1::i3
uint32 Parent1::i4
uint32[4] Parent1::i5
uint32*[4][8] Parent1::i6
float32 Parent2::f1
float64 Parent2::f2
bool Child::b
__m128 Child::c
std::basic_string<char,std::char_traits<char>,std::allocator<char> > Child::str
*/




struct EMVContext
{
    HANDLE hprocess;
    ULONG64 modbase;
    std::string this_type;
    std::string current_class;
    std::string current_type;
    std::string current_value;
    std::string tmp_name;
    MemberInfo mi;

    EMVContext()
        : hprocess(::GetCurrentProcess())
        , modbase((ULONG64)::GetModuleHandleA(nullptr))
    {
        this_type.reserve(MAX_SYM_NAME);
        current_class.reserve(MAX_SYM_NAME);
        current_type.reserve(MAX_SYM_NAME);
        current_value.reserve(MAX_SYM_NAME);
        tmp_name.reserve(MAX_SYM_NAME);
        memset(&mi, 0, sizeof(mi));
    }

    void updateMemberInfo()
    {
        mi.this_type  = this_type.c_str();
        mi.class_name = current_class.c_str();
        mi.type_name  = current_type.c_str();
        mi.value_name = current_value.c_str();
    }
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

void EnumMemberVariables(EMVContext &ctx, DWORD t, const MemberInfoCallback &f)
{
    DWORD tag;
    ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMTAG, &tag );

    if(tag==SymTagData) {
        DWORD tid = GetSymbolTypeID(ctx, t);
        GetSymbolTypeName(ctx, tid);
        ctx.current_type = ctx.tmp_name;
        GetSymbolName(ctx, t);
        ctx.current_value = ctx.tmp_name;
        ctx.updateMemberInfo();
        f(ctx.mi);
    }
    else if(tag==SymTagBaseClass) {
        DWORD type;
        if(::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPE, &type)) {
            EnumMemberVariables(ctx, type, f);
        }
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
                    EnumMemberVariables(ctx, params->ChildId[i], f);
                }
            }
            free(params);
        }
        ctx.current_class = prev;
    }
}



bool EnumMemberVariablesImpl(EMVContext &ctx, const MemberInfoCallback &f)
{
    ULONG tindex = 0;
    bool ok = false;
    {
        char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME];
        PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
        sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        sinfo->MaxNameLen = MAX_SYM_NAME;

        if(::SymGetTypeFromName(ctx.hprocess, ctx.modbase, ctx.this_type.c_str(), sinfo)) {
            ok = true;
            tindex = sinfo->TypeIndex;
        }
    }
    if(ok) {
        EnumMemberVariables(ctx, tindex, f);
    }
    return ok;
}

bool EnumMemberVariablesByTypeName(const char *classname, const MemberInfoCallback &f)
{
    EMVContext ctx;
    ctx.this_type = classname;
    return EnumMemberVariablesImpl(ctx, f);
}


bool EnumMemberVariablesByPointer(void *_this, const MemberInfoCallback &f)
{
    EMVContext ctx;
    {
        char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME];
        PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
        sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        sinfo->MaxNameLen = MAX_SYM_NAME;

        // vftable のシンボル名が "class名::`vftable'" になっているので、そこから class 名を取得
        if(::SymFromAddr(ctx.hprocess, (DWORD64)((void***)_this)[0], nullptr, sinfo)) {
            char vftable[MAX_SYM_NAME];
            ::UnDecorateSymbolName(sinfo->Name, vftable, MAX_SYM_NAME, UNDNAME_NAME_ONLY);
            if(char *colon=strstr(vftable, "::`vftable'")) {
                *colon = '\0';
                ctx.mi.this_pointer = _this;
                ctx.this_type = vftable;
            }
        }
    }
    if(!ctx.this_type.empty()) {
        return EnumMemberVariablesImpl(ctx, f);
    }
    return false;
}


