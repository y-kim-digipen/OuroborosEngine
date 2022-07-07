#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

struct SampleInfo {
    unsigned HitCount_;
    unsigned __int64 Addr_;
    std::string SymbolName_;
    double TimeDuration_;

};

class Profiler {
public:
    Profiler();
    ~Profiler();

    static void Init();
    static void Start();
    static void Exit();
    static void Sample();

    static HANDLE MainThread_;
    static DWORD MainThreadId_;
    static std::thread* WorkerThread_;

    static unsigned SampleNumber_;
};
