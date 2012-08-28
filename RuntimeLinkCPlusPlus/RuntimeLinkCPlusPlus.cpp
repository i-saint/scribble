// .obj ファイルを実行時にロード＆リンクして実行する試み。
// 以下のような制限はあるものの、とりあえず目的は果たせているように見えます。
// 
// ・/GL でコンパイルした .obj は読めない (リンク時の関数inline 展開実現のためにフォーマットが変わるらしい)
// ・exe 本体のデバッグ情報 (.pdb) が必要 (実行時リンクの際に文字列から関数のアドレスを取れないといけないので)
// ・exe 本体が import してない外部 dll の関数は呼べない (.lib 読んで超頑張ればできそうだがあまりに面倒…)
// ・.obj から exe の関数を呼ぶ場合、inline 展開とか最適化で消えてないかとかに注意が必要 (__declspec(dllexport) つければ対処可能)
// ・.obj から関数を引っ張ってくる際、mangling 後の関数名を指定する必要がある (extern "C" なら "_" をつけるだけだが、そうでない場合面倒)
// 

#include "stdafx.h"
#include <windows.h>
#include <imagehlp.h>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include "Interface.h"
#pragma comment(lib, "imagehlp.lib")
#pragma warning(disable: 4996) // _s じゃない CRT 関数使うとでるやつ

namespace stl = std;




template<size_t N>
inline int istsprintf(char (&buf)[N], const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    int r = _vsnprintf(buf, N, format, vl);
    va_end(vl);
    return r;
}

template<size_t N>
inline int istvsprintf(char (&buf)[N], const char *format, va_list vl)
{
    return _vsnprintf(buf, N, format, vl);
}

#define istPrint(...) DebugPrint(__FILE__, __LINE__, __VA_ARGS__)

static const int DPRINTF_MES_LENGTH  = 4096;
void DebugPrintV(const char* /*file*/, int /*line*/, const char* fmt, va_list vl)
{
    char buf[DPRINTF_MES_LENGTH];
    //istsprintf(buf, "%s:%d - ", file, line);
    //::OutputDebugStringA(buf);
    //WriteLogFile(buf);
    istvsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
}

void DebugPrint(const char* file, int line, const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    DebugPrintV(file, line, fmt, vl);
    va_end(vl);
}

bool InitializeDebugSymbol(HANDLE proc=::GetCurrentProcess())
{
    if(!::SymInitialize(proc, NULL, TRUE)) {
        return false;
    }
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    return true;
}




bool MapFile(const char *path, stl::vector<char> &data)
{
    if(FILE *f=fopen(path, "rb")) {
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        if(size > 0) {
            data.resize(size);
            fseek(f, 0, SEEK_SET);
            fread(&data[0], 1, size, f);
        }
        fclose(f);
        return true;
    }
    return false;
}

void MakeExecutable(void *p, size_t size)
{
    DWORD old_flag;
    VirtualProtect(p, size, PAGE_EXECUTE_READWRITE, &old_flag);
}

void* FindSymbolInExe(const char *name)
{
    char buf[sizeof(SYMBOL_INFO)+MAX_PATH];
    PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
    sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    sinfo->MaxNameLen = MAX_PATH;
    if(SymFromName(::GetCurrentProcess(), name, sinfo)==FALSE) {
        return NULL;
    }
    return (void*)sinfo->Address;
}


class ObjLoader;

class ObjManager
{
public:
    ObjManager();
    ~ObjManager();

    // .obj のロードを行う。
    // 既に読まれているファイルを指定した場合リロード処理を行う。
    void load(const stl::string &path);

    // 依存関係の解決処理。ロード後実行前に必ず呼ぶ必要がある。
    // load の中で link までやってもいいが、.obj の数が増えるほど無駄が多くなる上、
    // 未解決シンボルを判別しづらくなるので手順を分割した。
    void link();

    // 全ロード済み obj からシンボルを検索
    void* findSymbol(const stl::string &name);

    // exe 側 obj 側問わずシンボルを探す。link 処理用
    void* resolveSymbol(const stl::string &name);

private:
    typedef stl::map<stl::string, ObjLoader*> ObjLoaderMap;
    typedef stl::map<stl::string, void*> SymbolTable;

    ObjLoaderMap m_loaders;
    SymbolTable m_symbols;
};

class ObjLoader
{
public:
    ObjLoader() {}
    ObjLoader(const char *path) { load(path); }

    void clear();

    bool load(const char *path);

    // 外部シンボルのリンケージ解決
    void link();

    void* findInternalSymbol(const char *name);
    void* findSymbol(const char *name);

    // f: functor [](const stl::string &symbol_name, const void *data)
    template<class F>
    void eachSymbol(const F &f)
    {
        for(SymbolTable::iterator i=m_symbols.begin(); i!=m_symbols.end(); ++i) {
            f(i->first, i->second);
        }
    }

private:
    typedef stl::map<stl::string, void*> SymbolTable;
    stl::vector<char> m_data;
    stl::string m_filepath;
    SymbolTable m_symbols;
};


void ObjLoader::clear()
{
    m_data.clear();
    m_filepath.clear();
    m_symbols.clear();
}

bool ObjLoader::load(const char *path)
{
    clear();
    m_filepath = path;
    if(!MapFile(path, m_data)) {
        return false;
    }

    size_t ImageBase = (size_t)(&m_data[0]);
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if( pDosHeader->e_magic!=IMAGE_FILE_MACHINE_I386 || pDosHeader->e_sp!=0 ) {
        return false;
    }

    // 以下 symbol 収集処理
    PIMAGE_FILE_HEADER pImageHeader = (PIMAGE_FILE_HEADER)ImageBase;
    PIMAGE_OPTIONAL_HEADER *pOptionalHeader = (PIMAGE_OPTIONAL_HEADER*)(pImageHeader+1);

    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(ImageBase + sizeof(IMAGE_FILE_HEADER) + pImageHeader->SizeOfOptionalHeader);
    PIMAGE_SYMBOL pSymbolTable = (PIMAGE_SYMBOL)((size_t)pImageHeader + pImageHeader->PointerToSymbolTable);
    DWORD SymbolCount = pImageHeader->NumberOfSymbols;

    PSTR StringTable = (PSTR)&pSymbolTable[SymbolCount];

    for( size_t i=0; i < SymbolCount; ++i ) {
        IMAGE_SYMBOL &sym = pSymbolTable[i];
        if(sym.N.Name.Short == 0 && sym.SectionNumber>0) {
            IMAGE_SECTION_HEADER &sect = pSectionHeader[sym.SectionNumber-1];
            const char *name = (const char*)(StringTable + sym.N.Name.Long);
            void *data = (void*)(ImageBase + sect.PointerToRawData);
            if(sym.SectionNumber!=IMAGE_SYM_UNDEFINED) {
                MakeExecutable(data, sect.SizeOfRawData);
                m_symbols[name] = data;
            }
        }
        i += pSymbolTable[i].NumberOfAuxSymbols;
    }

    return true;
}

// 外部シンボルのリンケージ解決
void ObjLoader::link()
{
    size_t ImageBase = (size_t)(&m_data[0]);
    PIMAGE_FILE_HEADER pImageHeader = (PIMAGE_FILE_HEADER)ImageBase;
    PIMAGE_OPTIONAL_HEADER *pOptionalHeader = (PIMAGE_OPTIONAL_HEADER*)(pImageHeader+1);

    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(ImageBase + sizeof(IMAGE_FILE_HEADER) + pImageHeader->SizeOfOptionalHeader);
    PIMAGE_SYMBOL pSymbolTable = (PIMAGE_SYMBOL)((size_t)pImageHeader + pImageHeader->PointerToSymbolTable);
    DWORD SymbolCount = pImageHeader->NumberOfSymbols;

    PSTR StringTable = (PSTR)&pSymbolTable[SymbolCount];

    for( size_t i=0; i < SymbolCount; ++i ) {
        IMAGE_SYMBOL &sym = pSymbolTable[i];
        if(sym.N.Name.Short == 0 && sym.SectionNumber>0) {
            IMAGE_SECTION_HEADER &sect = pSectionHeader[sym.SectionNumber-1];
            const char *name = (const char*)(StringTable + sym.N.Name.Long);
            size_t RawData = (size_t)(ImageBase + sect.PointerToRawData);

            DWORD NumRelocations = sect.NumberOfRelocations;
            PIMAGE_RELOCATION pRelocation = (PIMAGE_RELOCATION)(ImageBase + sect.PointerToRelocations);
            for(size_t ri=0; ri<NumRelocations; ++ri) {
                PIMAGE_RELOCATION pReloc = pRelocation + ri;
                PIMAGE_SYMBOL pSym = pSymbolTable + pReloc->SymbolTableIndex;
                const char *name = (const char*)(StringTable + pSym->N.Name.Long);
                void *symdata = findSymbol(name);
                if(symdata==NULL) {
                    istPrint("!danger! %s: シンボル %s を解決できませんでした。\n", m_filepath.c_str(), name);
                    continue;
                }
                unsigned char instruction = *(unsigned char*)(RawData + pReloc->VirtualAddress - 1);
                // jmp で飛ぶ場合は相対アドレスに変換
                if(instruction==0xE9) {
                    size_t rel = (size_t)symdata - RawData - 5;
                    symdata = (void*)rel;
                }
                *(void**)(RawData + pReloc->VirtualAddress) = symdata;
            }
        }
        i += pSymbolTable[i].NumberOfAuxSymbols;
    }
}

void* ObjLoader::findInternalSymbol(const char *name)
{
    SymbolTable::iterator i = m_symbols.find(name);
    if(i == m_symbols.end()) { return NULL; }
    return i->second;
}

void* ObjLoader::findSymbol(const char *name)
{
    void *ret = findInternalSymbol(name);
    if(!ret) { ret = FindSymbolInExe(name); }
    return ret;
}





// .obj から呼ぶ関数。最適化で消えないように dllexport つけておく
__declspec(dllexport) void FuncInExe()
{
    istPrint("FuncInExe()\n");
}




class Hoge : public IHoge
{
public:
    virtual void DoSomething()
    {
        istPrint("Hoge::DoSomething()\n");
    }
};

typedef float (*FloatOpT)(float, float);
FloatOpT FloatAdd = NULL;
FloatOpT FloatSub = NULL;
FloatOpT FloatMul = NULL;
FloatOpT FloatDiv = NULL;

typedef void (*CallExternalFuncT)();
CallExternalFuncT CallExternalFunc = NULL;
CallExternalFuncT CallExeFunc = NULL;

typedef void (*IHogeReceiverT)(IHoge*);
IHogeReceiverT IHogeReceiver = NULL;


int main(int argc, _TCHAR* argv[])
{
    InitializeDebugSymbol();

    ObjLoader objloader;
    if(!objloader.load("DynamicFunc.obj")) {
        return 1;
    }
    objloader.link();
    FloatAdd = (FloatOpT)objloader.findInternalSymbol("_FloatAdd");
    FloatSub = (FloatOpT)objloader.findInternalSymbol("_FloatSub");
    FloatMul = (FloatOpT)objloader.findInternalSymbol("_FloatMul");
    FloatDiv = (FloatOpT)objloader.findInternalSymbol("_FloatDiv");
    IHogeReceiver = (IHogeReceiverT)objloader.findInternalSymbol("_IHogeReceiver");
    CallExternalFunc = (CallExternalFuncT)objloader.findInternalSymbol("_CallExternalFunc");
    CallExeFunc = (CallExternalFuncT)objloader.findInternalSymbol("_CallExeFunc");

    istPrint("%.2f\n", FloatAdd(1.0f, 2.0f));
    istPrint("%.2f\n", FloatSub(1.0f, 2.0f));
    istPrint("%.2f\n", FloatMul(1.0f, 2.0f));
    istPrint("%.2f\n", FloatDiv(1.0f, 2.0f));
    {
        Hoge hoge;
        IHogeReceiver(&hoge);
    }

    CallExternalFunc();
    CallExeFunc();

    return 0;
}

