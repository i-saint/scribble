#ifndef rps_h
#define rps_h

#define rpsDLLExport __declspec(dllexport)
#define rpsDLLImport __declspec(dllimport)
#ifdef rpsImplDLL
#   define rpsAPI rpsDLLExport
#else  // rpsImplDLL
#   define rpsAPI rpsDLLImport
#   if defined(_M_IX86)
#       pragma comment(lib,"rps32.lib")
#   elif defined(_M_X64)
#       pragma comment(lib,"rps64.lib")
#   endif
#endif // rpsImplDLL

rpsAPI void rpsInitialize();
rpsAPI void rpsSaveState(const char *path_to_outfile);
rpsAPI void rpsLoadState(const char *path_to_infile);

#endif // rps_h
