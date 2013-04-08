cl /nologo /MD /Zi /EHs TestInject.cpp CompleteInject.cpp
cl /nologo /MD /Zi /EHs /LD TestDLL.cpp
cl /nologo /MD TestInjected.cpp
