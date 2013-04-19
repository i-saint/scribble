#include "IntermoduleSingleton.h"

class INTERMODULE TestClass : public IntermoduleSingleton<TestClass>
{
public:
    void DoSomething();
};
