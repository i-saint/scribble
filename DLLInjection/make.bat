cl /nologo /MT /Zi /EHs /LD TestDLL.cpp /link /opt:ref
cl /nologo /MD /Zi /EHs TestInjected.cpp /link /opt:ref
cl /nologo /MD /Zi /EHs ExecuteWithDLLInjection.cpp /link /opt:ref
