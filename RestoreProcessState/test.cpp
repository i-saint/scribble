#include "rps.h"
#include "rpsInlines.h"

int main(int argc, char *argv[])
{
    rpsProcessState::initialize();

    std::string *ptest = new std::string("abc");
    std::string &test = *ptest;
    printf("before serialize. test: %s\n", test.c_str());

    rpsProcessState::getInstance()->serialize("test.stat", rpsArchive::Writer);
after_serialization:

    printf("after serialize. test: %s\n", test.c_str());
    test += "def";
    printf("after test+=\"def\". test: %s\n", test.c_str());

    ::Sleep(2000);
    rpsProcessState::getInstance()->serialize("test.stat", rpsArchive::Reader);
    goto after_serialization;

}
