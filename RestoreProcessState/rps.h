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

template<size_t Size>
struct rpsAny
{
	template<class T> T& operator=(const T &v) { return cast<T>()=v; }
	template<class T>     void dtor()       { ((T&)*buf).~T(); }
	template<class T>       T& cast()       { return (T&)*buf; }
	template<class T> const T& cast() const { return (T&)*buf; }
	char buf[Size];
};
typedef rpsAny<64> rpsAny64;

struct rpsMessage
{
	char modulename[32];
	char command[32];
	rpsAny64 value;

	rpsMessage() {}
	rpsMessage(const char *mod, const char *cmd)
	{
		strncpy_s(modulename, sizeof(modulename), mod, sizeof(modulename));
		strncpy_s(command, sizeof(modulename), cmd, sizeof(command));
	}
	template<class T>
	rpsMessage(const char *mod, const char *cmd, const T &v)
	{
		strncpy_s(modulename, sizeof(modulename), mod, sizeof(modulename));
		strncpy_s(command, sizeof(modulename), cmd, sizeof(command));
		value = v;
	}
};


rpsAPI void rpsInitialize();
rpsAPI void rpsSaveState(const char *path_to_outfile);
rpsAPI void rpsLoadState(const char *path_to_infile);
rpsAPI void rpsSendMessage(rpsMessage &mes);

#endif // rps_h
