#include "rps.h"
#include "rpsInlines.h"

int main(int argc, char *argv[])
{
    rpsProcessState::initialize();

    std::string test("abc");
    printf("before serialize. test: %s\n", test.c_str());

    rpsProcessState::getInstance()->serialize("test.stat", rpsArchive::Writer);

    printf("after serialize. test: %s\n", test.c_str());
    test += "def";
    printf("after test+=\"def\". test: %s\n", test.c_str());

    rpsProcessState::getInstance()->serialize("test.stat", rpsArchive::Reader);

    return 0;
}
