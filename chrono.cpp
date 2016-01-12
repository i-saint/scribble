#include <chrono>
#include <ctime>
#include <cstdio>
#include <cstdint>
#include <windows.h>


// std::chrono::steady_clock::now() as nanoseconds
uint64_t StdChrono_StreadyClock()
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

// std::chrono::system_clock::now() as nanoseconds
uint64_t StdChrono_SystemClock()
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

// equivalant to std::chrono::steady_clock::now() as nanoseconds
uint64_t WinAPI_QPC()
{
    static LARGE_INTEGER g_freq = { 0, 0 };
    if (g_freq.QuadPart == 0) {
        ::QueryPerformanceFrequency(&g_freq);
    }

    LARGE_INTEGER t;
    ::QueryPerformanceCounter(&t);
    return uint64_t(double(t.QuadPart) / double(g_freq.QuadPart) * 1000000000.0);
}

// equivalant to std::chrono::system_clock::now() as nanoseconds
uint64_t WinAPI_GSTAFT()
{
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    const int64_t EPOCH_BIAS = 116444736000000000i64;
    return ((int64_t&)ft - EPOCH_BIAS) * 100;
}


int main(int argc, char *argv[])
{
    for (int i = 0; i < 10; ++i) {
        uint64_t now_chrono = StdChrono_StreadyClock();
        uint64_t now_winapi = WinAPI_QPC();
        printf("steady_clock: %llu  QPC: %llu\n", now_chrono, now_winapi);
        ::Sleep(100);
    }
    printf("\n");
    for (int i = 0; i < 10; ++i) {
        uint64_t now_chrono = StdChrono_SystemClock();
        uint64_t now_winapi = WinAPI_GSTAFT();
        printf("system_clock: %llu  GSTAFT: %llu\n", now_chrono, now_winapi);
        ::Sleep(100);
    }
}

/*
result:

steady_clock: 611332070311940  QPC: 611332070312196
steady_clock: 611332170789019  QPC: 611332170789275
steady_clock: 611332270826037  QPC: 611332270827061
steady_clock: 611332371176141  QPC: 611332371176909
steady_clock: 611332471383398  QPC: 611332471384166
steady_clock: 611332571740926  QPC: 611332571741694
steady_clock: 611332672186518  QPC: 611332672187286
steady_clock: 611332772267568  QPC: 611332772268336
steady_clock: 611332872361161  QPC: 611332872361673
steady_clock: 611332972483682  QPC: 611332972484706

system_clock: 1452594687765373500  GSTAFT: 1452594687765364100
system_clock: 1452594687865597700  GSTAFT: 1452594687865576200
system_clock: 1452594687965710400  GSTAFT: 1452594687965675900
system_clock: 1452594688065984600  GSTAFT: 1452594688065963100
system_clock: 1452594688166448500  GSTAFT: 1452594688166415300
system_clock: 1452594688266675800  GSTAFT: 1452594688266628200
system_clock: 1452594688366947200  GSTAFT: 1452594688366936800
system_clock: 1452594688467198100  GSTAFT: 1452594688467130300
system_clock: 1452594688567382300  GSTAFT: 1452594688567374400
system_clock: 1452594688667782500  GSTAFT: 1452594688667776700
*/