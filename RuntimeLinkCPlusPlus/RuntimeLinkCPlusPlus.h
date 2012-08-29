#ifndef __RuntimeLinkCPlusPlus_h__
#define __RuntimeLinkCPlusPlus_h__


#define RLCPP_Enable_Dynamic_Link


#ifdef RLCPP_Enable_Dynamic_Link

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
