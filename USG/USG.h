#ifndef USG_h
#define USG_h

#define _NO_CVCONST_H
#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")




namespace usg {

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


    class Generator
    {
    public:
        static void execute();

        Generator(const char *path_to_source, int line, const char *classname);
        void generate();

    private:
        static std::vector<Generator*>& getInstances() { static std::vector<Generator*> s_generators; return s_generators; }
        const char *m_file;
        const char *m_classname;
        int m_line;
    };

    inline void DebugPrintV(const char* fmt, va_list vl)
    {
        char buf[1024];
        _vsnprintf_s(buf, _countof(buf), fmt, vl);
        ::OutputDebugStringA(buf);
    }
    inline void DebugPrint(const char* fmt, ...)
    {
        va_list vl;
        va_start(vl, fmt);
        DebugPrintV(fmt, vl);
        va_end(vl);
    }

    Generator::Generator( const char *path_to_source, int line, const char *classname )
        : m_file(path_to_source)
        , m_classname(classname)
        , m_line(line)
    {
        getInstances().push_back(this);
    }

    void Generator::generate()
    {
        std::string str;
        {
            FILE *f = NULL;
            fopen_s(&f, m_file, "rb");
            if(f==NULL) {
                DebugPrint("USG error: failed to open %s\n", m_file);
                return;
            }
            fseek(f, 0, SEEK_END);
            str.resize(ftell(f));
            fseek(f, 0, SEEK_SET);
            fread(&str[0], 1, str.size(), f);
            fclose(f);
        }

        EnumMemberVariables("Hage", [](std::string &name){
            printf("%s\n", name.c_str());
        });

        std::smatch m1;
        if(std::regex_search(str, m1, std::regex(std::string("USG_GenerateBlockBegin\\s*\\(\\s*") + m_classname + "\\s*\\)"))) {
            for(std::smatch::iterator i1=m1.begin(); i1!=m1.end(); ++i1) {
                std::smatch m2;
                std::regex r("USG_SerializeBlockBegin\\s*\\(\\s*([^)]+)\\s*\\)");
                if(std::regex_search(std::string(i1->str(), i1->length()), m2, r)) {

                }
                DebugPrint("%d\n", m1.position());

            }
        }
    }

    void Generator::execute()
    {
        ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
        ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

        for(size_t i=0; i<getInstances().size(); ++i) {
            getInstances()[i]->generate();
        }

        ::ExitProcess(0);
    }



} // namespace usg

#ifdef USG_Generate
#   define USG_DefineGenerater(Functor)                 
#   define USG_ExecGenerate                             usg::Generator::execute()
#   define USG_GenerateBlockBegin(ClassName)            static usg::Generator g_usg_generator_##__LINE__##(__FILE__, __LINE__, #ClassName);
#   define USG_GenerateBlockEnd()
#else // USG_Generate
#   define USG_ExecGenerate
#   define USG_GenerateBlockBegin(ClassName)
#   define USG_GenerateBlockEnd()
#endif // USG_Generate
#define USG_SerializeBlockBegin(Serializer)
#define USG_SerializeBlockEnd()
#define USG_DeserializeBlockBegin(Deserializer)
#define USG_DeserializeBlockEnd()


#endif // USG_h
