// RuntimeLinkCPlusPlus.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
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




bool MapFile(const char *path, std::vector<char> &data)
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


class ObjLoader
{
public:
    ObjLoader() {}
    ObjLoader(const char *path) { load(path); }

    void clear()
    {
        m_data.clear();
        m_symbols.clear();
    }

    bool load(const char *path)
    {
        clear();
        if(!MapFile(path, m_data)) {
            return false;
        }

        size_t ImageBase = (size_t)(&m_data[0]);
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
        if( pDosHeader->e_magic!=IMAGE_FILE_MACHINE_I386 || pDosHeader->e_sp!=0 ) {
            return false;
        }

        PIMAGE_FILE_HEADER pImageHeader = (PIMAGE_FILE_HEADER)ImageBase;
        PIMAGE_OPTIONAL_HEADER *pOptionalHeader = (PIMAGE_OPTIONAL_HEADER*)(pImageHeader+1);

        PIMAGE_SYMBOL pSymbolTable = (PIMAGE_SYMBOL)((size_t)pImageHeader + pImageHeader->PointerToSymbolTable);
        DWORD SymbolCount = pImageHeader->NumberOfSymbols;

        PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(ImageBase + sizeof(IMAGE_FILE_HEADER) + pImageHeader->SizeOfOptionalHeader);
        DWORD SectionCount = pImageHeader->NumberOfSections;

        PSTR StringTable = (PSTR)&pSymbolTable[SymbolCount];

        // symbol 収集フェイズ
        for( size_t i=0; i < SymbolCount; ++i ) {
            IMAGE_SYMBOL &sym = pSymbolTable[i];
            if(sym.N.Name.Short == 0) {
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

        // link フェイズ
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
                    if(void *d = findSymbol(name)) {
                        // jmp で飛ぶ場合は相対アドレスに変換
                        if(*(unsigned char*)(RawData + pReloc->VirtualAddress - 1)==0xE9) {
                            size_t rel = (size_t)d - RawData - 5;
                            d = (void*)rel;
                        }
                        *(void**)(RawData + pReloc->VirtualAddress) = d;
                    }
                }
            }
            i += pSymbolTable[i].NumberOfAuxSymbols;
        }

        return true;
    }


    void* findInternalSymbol(const char *name)
    {
        SymbolTable::iterator i = m_symbols.find(name);
        if(i == m_symbols.end()) { return NULL; }
        return i->second;
    }

    void* findExternalSymbol(const char *name)
    {
        char buf[sizeof(SYMBOL_INFO)+MAX_PATH];
        PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
        sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        sinfo->MaxNameLen = MAX_PATH;
        SymFromName(::GetCurrentProcess(), name, sinfo);
        return (void*)sinfo->Address;
    }

    void* findSymbol(const char *name)
    {
        void *ret = findInternalSymbol(name);
        if(!ret) { ret = findExternalSymbol(name); }
        return ret;
    }

    // f: functor [](const std::string &symbol_name, const void *data)
    template<class F>
    void eachSymbol(const F &f)
    {
        for(SymbolTable::iterator i=m_symbols.begin(); i!=m_symbols.end(); ++i) {
            f(i->first, i->second);
        }
    }

private:
    typedef std::map<std::string, void*> SymbolTable;
    std::vector<char> m_data;
    SymbolTable m_symbols;
};

__declspec(dllexport) void FuncInExe()
{
    istPrint("FuncInExe()\n");
}




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

class Hoge : public IHoge
{
public:
    virtual void DoSomething()
    {
        istPrint("Hoge::DoSomething()\n");
    }
};


int main(int argc, _TCHAR* argv[])
{
    InitializeDebugSymbol();

    ObjLoader objloader;
    if(!objloader.load("DynamicFunc.obj")) {
        return 1;
    }
    objloader.eachSymbol([](const std::string &name, const void *data){
        if     (name=="_FloatAdd")          { FloatAdd = (FloatOpT)data; }
        else if(name=="_FloatSub")          { FloatSub = (FloatOpT)data; }
        else if(name=="_FloatMul")          { FloatMul = (FloatOpT)data; }
        else if(name=="_FloatDiv")          { FloatDiv = (FloatOpT)data; }
        else if(name=="_IHogeReceiver")     { IHogeReceiver = (IHogeReceiverT)data; }
        else if(name=="_CallExternalFunc")  { CallExternalFunc = (CallExternalFuncT)data; }
        else if(name=="_CallExeFunc")       { CallExeFunc = (CallExternalFuncT)data; }
    });

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

