#include <thread>

#include "fmtlog.h"
// #include "fmt/core.h"
// #include "fmt/ranges.h"
#include <iostream>

int main(int argc, char** argv) {
    fmtlog::setLogLevel(fmtlog::LogLevel::DBG);
    {
       auto srrr = fmt::format("hello {}", "world");
       logd("bhjbhjbhjbh");
        //logd("asdasd");
        std::cout << srrr << "\n";
    }
    fmtlog::poll();
    return 0;
}