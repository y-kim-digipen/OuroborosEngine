#pragma once
#include <cstdint>
#include <iostream>

#ifndef NDEBUG
#   define OE_Assert(Expr, Msg) \
    __OE_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#   define OE_Assert(Expr, Msg) ;
#endif

void __OE_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg)
{
    if (!expr)
    {
        std::cerr << "Assert failed:\t" << msg << "\n"
            << "Expected:\t" << expr_str << "\n"
            << "Source:\t\t" << file << ", line " << line << "\n";
        abort();
    }
}

namespace OE
{
    using ecs_ID = int16_t;
    static_assert(sizeof(ecs_ID) == 2 && std::is_signed_v<ecs_ID>,
        "ID type mismatch");
}
