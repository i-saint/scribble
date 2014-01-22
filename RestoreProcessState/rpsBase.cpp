#include "rps.h"
#include "rpsInlines.h"

extern rpsIModule* rpsCreateMemory();
extern rpsIModule* rpsCreateThreads();
extern rpsIModule* rpsCreateFiles();

static rpsModuleCreator g_mcreators[] = {
	rpsCreateMemory,
	rpsCreateThreads,
	rpsCreateFiles,
};

rpsDLLExport size_t				rpsGetNumModuleCreators()	{ return _countof(g_mcreators); }
rpsDLLExport rpsModuleCreator*	rpsGetModuleCreators()		{ return g_mcreators; }



rpsArchive::rpsArchive()
	: m_file(nullptr)
	, m_is_reader(false)
{
}

rpsArchive::~rpsArchive()
{
	close();
}

void rpsArchive::read(void *dst, size_t size)
{
	fread(dst, size, 1, m_file);
}

void rpsArchive::write(const void *data, size_t size)
{
	fwrite(data, size, 1, m_file);
}

void rpsArchive::io(void *dst, size_t size)
{
	if(isReader()) {
		read(dst, size);
	}
	else {
		write(dst, size);
	}
}

bool rpsArchive::open(const char *path_to_file, bool is_reader)
{
	close();
	m_file = fopen(path_to_file, is_reader ? "rb" : "wb");
	return m_file!=nullptr;
}

void rpsArchive::close()
{
	if(m_file) {
		fclose(m_file);
	}
}



rpsProcessState* rpsProcessState::getInstance()
{
	static rpsProcessState *g_procstate = nullptr;
	if(!g_procstate) {
		g_procstate = new rpsProcessState();
	}
	return g_procstate;
}

rpsProcessState::rpsProcessState()
{
	rpsModuleCreator *ctab = rpsGetModuleCreators();
	size_t n = rpsGetNumModuleCreators();
	for(size_t i=0; i<n; ++i) {
		rpsIModule *mod = ctab[i]();
		m_modules[mod->getModuleName()] = mod;
	}
}

rpsProcessState::~rpsProcessState()
{
}

void rpsProcessState::serialize(rpsArchive &ar)
{
	eachModules([&](rpsIModule *mod){
		mod->serialize(ar);
	});
}




//void rpsInitialize()
//{
//	rpsEnumerateModules([](HMODULE mod){
//		rpsEnumerateDLLImports(mod, "kernel32.dll", [&](const char *name, void *&func){
//			if     (strcmp(name, "HeapAlloc")	==0) { rpsForceWrite<void*>(func, rpsHeapAlloc); }
//			else if(strcmp(name, "HeapReAlloc")	==0) { rpsForceWrite<void*>(func, rpsHeapReAlloc); }
//			else if(strcmp(name, "HeapFree")	==0) { rpsForceWrite<void*>(func, rpsHeapFree); }
//		});
//	});
//}
//
//
//void rpsRestore()
//{
//	CONTEXT ctx;
//}

