// .obj ファイルを実行時にロード＆リンクして実行する試み。
// 以下のような制限はあるものの、とりあえず目的は果たせているように見えます。
// 
// ・/GL でコンパイルした .obj は読めない
//      リンク時の関数inline 展開実現のためにフォーマットが変わるらしい
// ・exe 本体のデバッグ情報 (.pdb) が必要
//      実行時リンクの際に文字列から関数のアドレスを取れないといけないので
// ・exe 本体が import してない外部 dll の関数は呼べない
//      .lib 読んで超頑張ればできそうだがあまりに面倒…
// ・.obj から exe の関数を呼ぶ場合、対象が inline 展開されてたり最適化で消えてたりすると、あらぬところに jmp してクラッシュする
//      とりあえず __declspec(dllexport) つければ対処可能
// ・virtual 関数を使う場合、RTTI を無効にしておく必要がある
//      RTTI の有無で vftable の内容が変わってしまうので。対処したいが方法がよくわからず。
// ・.obj から関数を引っ張ってくる際、mangling 後の関数名を指定する必要がある
//      とりあえずこのライブラリは extern "C" で解決している。(C linkage の場合 "_" をつけるだけで済む)
// ・.obj 側のコードはデバッガでは逆アセンブルモードでしか追えない
//      デバッグ情報はロードしていないため。これは解決困難で諦めモード。

#ifndef __RuntimeLinkCPlusPlus_h__
#define __RuntimeLinkCPlusPlus_h__


#define RLCPP_Enable_Dynamic_Link


#if defined(_WIN32) && defined(RLCPP_Enable_Dynamic_Link)

void  _RLCPP_InitializeLoader();
void  _RLCPP_FinalizeLoader();
void  _RLCPP_Load(const char *path);
void  _RLCPP_Link();
void* _RLCPP_FindSymbol(const char *name);

template<class FuncPtr>
inline void _RLCPP_GetFunction_Helper(FuncPtr &v, void *sym) { v=(FuncPtr)sym; }

#define RLCPP_ObjExport                         extern "C"
#define RLCPP_Fixate                            __declspec(dllexport)
#define RLCPP_DeclareObjFunc(ret, name, ...)    extern ret (*name)(__VA_ARGS__)
#define RLCPP_DefineObjFunc(ret, name, ...)     ret (*name)(__VA_ARGS__) = NULL
#define RLCPP_ObjValue(type, name)              type *name = NULL
#define RLCPP_GetFunction(name)                 _RLCPP_GetFunction_Helper(name, _RLCPP_FindSymbol("_" #name))
#define RLCPP_OnLoad(...)                       RLCPP_ObjExport void RLCPP_OnLoadHandler()   { __VA_ARGS__ }
#define RLCPP_OnUnload(...)                     RLCPP_ObjExport void RLCPP_OnUnloadHandler() { __VA_ARGS__ }

#define RLCPP_InitializeLoader()                _RLCPP_InitializeLoader()
#define RLCPP_FinalizeLoader()                  _RLCPP_FinalizeLoader()
#define RLCPP_Load(path)                        _RLCPP_Load(path)
#define RLCPP_Link()                            _RLCPP_Link()
#define RLCPP_FindSymbol(name)                  _RLCPP_FindSymbol(name)


#else // RLCPP_Enable_Dynamic_Link

#define RLCPP_ObjExport                         
#define RLCPP_Fixate                            
#define RLCPP_DeclareObjFunc(ret, name, ...)    ret name(__VA_ARGS__)
#define RLCPP_DefineObjFunc(ret, name, ...)     ret name(__VA_ARGS__)
#define RLCPP_GetFunction(name)                 
#define RLCPP_OnLoad(...)                       
#define RLCPP_OnUnload(...)                     

#define RLCPP_InitializeLoader()                
#define RLCPP_FinalizeLoader()                  
#define RLCPP_Load(path)                        
#define RLCPP_Link()                            
#define RLCPP_FindSymbol(name)                  


#endif // RLCPP_Enable_Dynamic_Link

#endif // __RuntimeLinkCPlusPlus_h__
