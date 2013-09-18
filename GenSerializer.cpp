#include <cstdio>
#include <string>
#include <regex>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstdint>

#define _NO_CVCONST_H
#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

enum MemberType {
    MT_Unknown,
    MT_Variable,
    MT_BaseClass,
    MT_Function,
};

struct MemberInfo
{
    MemberType type;
    void *this_pointer;     // this
    void *base_pointer;     // 親 class のメンバを指してる場合、親 class の先頭を指す
    void *value;            // メンバ変数へのポインタ
    std::string this_type;  // this の型
    std::string class_name; // メンバ変数が所属する class。親 class のメンバの場合 this_type とは違うものになる
    std::string type_name;  // メンバ変数の型名
    std::string value_name; // メンバ変数の名前

    MemberInfo() : type(MT_Unknown), this_pointer(), base_pointer(), value() {}
};
typedef std::function<void (const MemberInfo*, size_t)> MemberInfoCallback;

bool EnumMemberVariablesByTypeName(const char *classname, const MemberInfoCallback &f);
bool EnumMemberVariablesByTypeName(const char *classname, void *_this, const MemberInfoCallback &f);
bool EnumMemberVariablesByPointer(void *_this, const MemberInfoCallback &f);

class _GenSerializerProcess
{
public:
    typedef std::vector<_GenSerializerProcess*> instance_cont;
    typedef std::function<std::string (const char*, const MemberInfo*, size_t)> callback_t;

    _GenSerializerProcess(const char *path, const callback_t &cb);
    void process();
    static void exec();
private:
    static instance_cont& getInstances();
    const char *m_path;
    callback_t m_callback;
};

#define GenSerializerBegin(ClassName)   
#define GenSerializerEnd()              
#define GenSerializerProcess(...)       static _GenSerializerProcess g_gsp##__LINE__(__FILE__, __VA_ARGS__)
#define GenSerializerExec()             _GenSerializerProcess::exec()

// test

class Parent
{
public:
    int32_t     i1;
    int32_t     i2[4];
    float       f1;
    double      f2;
    template<class A> void serialize(A &ar, const uint32_t version);
};

class Child : public Parent
{
public:
    __m128      v;
    std::string str;
    template<class A> void serialize(A &ar, const uint32_t version);
};

GenSerializerBegin(Parent)
GenSerializerEnd()

GenSerializerBegin(Child)
GenSerializerEnd()

/*
↑が自動生成でこうなる。
boost::serialization を想定した例。

GenSerializerBegin(Parent)
template<class A>
void Parent::serialize(A &ar, const uint32_t version) {
    ar & i1;
    ar & i2;
    ar & f1;
    ar & f2;
}
GenSerializerEnd()

GenSerializerBegin(Child)
template<class A>
void Child::serialize(A &ar, const uint32_t version) {
    ar & boost::serialization::base_object<Parent>(*this);
    ar & v;
    ar & str;
}
GenSerializerEnd()
*/

GenSerializerProcess(
    [](const char *class_name, const MemberInfo *members, size_t num){
        std::string ret;
        char buf[4096];
        sprintf(buf,
            "template<class A>\r\n"
            "void %s::serialize(A &ar, const uint32_t version) {\r\n", class_name);
        ret += buf;
        for(size_t i=0; i<num; ++i) {
            const MemberInfo &mi = members[i];
            if(mi.type==MT_BaseClass) {
                sprintf(buf, "    ar & boost::serialization::base_object<%s>(*this);\r\n", mi.class_name.c_str());
                ret+=buf;
            }
            else if(mi.type==MT_Variable && mi.class_name==mi.this_type) {
                sprintf(buf, "    ar & %s;\r\n", mi.value_name.c_str());
                ret+=buf;
            }
        }
        ret+="}\r\n";
        return ret;
    }
);

int main(int argc, char* argv[])
{
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
    GenSerializerExec();

    Parent *p = new Child(); // 最適化によるシンボル消失抑止
    return 0;
}

/*
> cl /EHsc /Zi GenSerializer.cpp
> GenSerializer

*/






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

struct EMVContext
{
    HANDLE hprocess;
    ULONG64 modbase;
    MemberInfo current;
    std::vector<MemberInfo> members;
    std::string tmp_name;

    EMVContext()
        : hprocess(::GetCurrentProcess())
        , modbase((ULONG64)::GetModuleHandleA(nullptr))
    {
    }
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

bool GetSymbolTypeNameImpl(EMVContext &ctx, DWORD t, std::string &ret)
{
    DWORD tag = 0;
    DWORD basetype = 0;
    if(!::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMTAG, &tag)) {
        return false;
    }

    if(tag==SymTagArrayType) {
        DWORD count = 0;
        ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_COUNT, &count);
        char a[128];
        sprintf(a, "[%d]", count);
        ret += a;

        DWORD tid = 0;
        ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPEID, &tid);
        return GetSymbolTypeNameImpl(ctx, tid, ret);
    }
    else if(tag==SymTagPointerType) {
        ret = "*"+ret;

        DWORD tid = 0;
        ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPEID, &tid);
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
    else { // user defined type
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
    DWORD tag = 0;
    if(!::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_SYMTAG, &tag)) {
        return;
    }

    if(tag==SymTagData) {
        DWORD offset = 0;
        DWORD tid = 0;
        char type_name[MAX_SYM_NAME];
        char value_name[MAX_SYM_NAME];
        if( ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_OFFSET, &offset) &&
            ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPEID, &tid) )
        {
            ctx.current.value = (void*)((size_t)ctx.current.base_pointer+offset);
            GetSymbolTypeName(ctx, tid);
            ctx.current.type_name = ctx.tmp_name;
            GetSymbolName(ctx, t);
            ctx.current.value_name = ctx.tmp_name;
            ctx.current.type = MT_Variable;
            ctx.members.push_back(ctx.current);
            //f(ctx.mi);
        }
    }
    else if(tag==SymTagBaseClass) {
        void *base_prev = ctx.current.base_pointer;
        DWORD offset = 0;
        DWORD type = 0;
        if( ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_OFFSET, &offset) &&
            ::SymGetTypeInfo(ctx.hprocess, ctx.modbase, t, TI_GET_TYPE, &type) )
        {
            ctx.current.base_pointer = (void*)((size_t)base_prev+offset);
            EnumMemberVariables(ctx, type, f);
            ctx.current.base_pointer = base_prev;
        }
    }
    else if(tag==SymTagUDT) {
        std::string prev = ctx.current.class_name;
        GetSymbolName(ctx, t);
        ctx.current.class_name = ctx.tmp_name;
        if(ctx.current.this_type!=ctx.current.class_name) {
            ctx.current.type = MT_BaseClass;
            ctx.current.value = nullptr;
            ctx.current.value_name = "";
            ctx.current.type_name = "";
            ctx.members.push_back(ctx.current);
        }

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
        ctx.current.class_name = prev;
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

        if(::SymGetTypeFromName(ctx.hprocess, ctx.modbase, ctx.current.this_type.c_str(), sinfo)) {
            ok = true;
            tindex = sinfo->TypeIndex;
        }
    }
    if(ok) {
        EnumMemberVariables(ctx, tindex, f);
        f(&ctx.members[0], ctx.members.size());
    }
    return ok;
}

bool EnumMemberVariablesByTypeName(const char *classname, const MemberInfoCallback &f)
{
    EMVContext ctx;
    ctx.current.this_type = classname;
    return EnumMemberVariablesImpl(ctx, f);
}

bool EnumMemberVariablesByTypeName(const char *classname, void *_this, const MemberInfoCallback &f)
{
    EMVContext ctx;
    ctx.current.this_pointer = ctx.current.base_pointer = _this;
    ctx.current.this_type = classname;
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
                ctx.current.this_pointer = ctx.current.base_pointer = _this;
                ctx.current.this_type = vftable;
            }
        }
    }
    if(!ctx.current.this_type.empty()) {
        return EnumMemberVariablesImpl(ctx, f);
    }
    return false;
}


void scan(std::string content, std::regex pattern, const std::function<void (const std::cmatch&)> &f)
{
    std::cmatch m;
    size_t pos = 0;
    while(std::regex_search(content.c_str()+pos, m, pattern)) {
        f(m);
        pos += m.position()+m.length();
    }
}

std::string gsub(const std::string &content, const std::regex &pattern, const std::function<std::string (const std::cmatch&)> &f)
{
    struct replace_info {
        size_t pos, len;
        std::string replacement;
    };
    std::vector<replace_info> replacements;
    std::cmatch m;
    size_t pos = 0;
    while(std::regex_search(content.c_str()+pos, m, pattern)) {
        replace_info ri = {pos+m.position(), m.length(), f(m)};
        replacements.push_back(ri);
        pos += m.position()+m.length();
    }

    std::string ret = content;
    std::for_each(replacements.rbegin(), replacements.rend(), [&](replace_info &ri){
        ret.replace(ri.pos, ri.len, ri.replacement);
    });
    return ret;
}


_GenSerializerProcess::instance_cont& _GenSerializerProcess::getInstances()
{
    static instance_cont s_instances;
    return s_instances;
}

_GenSerializerProcess::_GenSerializerProcess(const char *path, const callback_t &cb)
{
    getInstances().push_back(this);
    m_path = path;
    m_callback = cb;
}

void _GenSerializerProcess::process()
{
    std::string cpp;
    if(FILE *fin=fopen(m_path, "rb")) {
        if(!fin) { return; }
        fseek(fin, 0, SEEK_END);
        cpp.resize((size_t)ftell(fin));
        fseek(fin, 0, SEEK_SET);
        fread(&cpp[0], 1, cpp.size(), fin);
        fclose(fin);
    }

    cpp = gsub(cpp, std::regex("^[ \\t]*GenSerializerBegin\\((.+?)\\)[.\\r\\n]+?GenSerializerEnd\\(\\)"), [&](const std::cmatch &m){
        std::string class_name = m[1].str();
        std::string code;
        EnumMemberVariablesByTypeName(class_name.c_str(), [&](const MemberInfo *mi, size_t num){
            code = m_callback(class_name.c_str(), mi, num);
        });

        std::string rep;
        rep += "GenSerializerBegin(";
        rep += class_name;
        rep += ")\r\n";
        rep += code;
        rep += "GenSerializerEnd()";
        return rep;
    });
    if(FILE *fout=fopen(m_path, "wb")) {
        fwrite(cpp.c_str(), cpp.size(), 1, fout);
        fclose(fout);
    }
}

void _GenSerializerProcess::exec()
{
    instance_cont& instances = getInstances();
    for(size_t i=0; i<instances.size(); ++i) {
        instances[i]->process();
    }
}

