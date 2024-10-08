//
// Created by johnk on 2021/12/19.
//

#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

#include <Common/Concepts.h>

#define DefaultCopyCtor(clz) \
    clz(clz&) = default; \
    clz(const clz&) = default; \

#define DefaultCopyAssignOp(clz) \
    clz& operator=(clz&) = default; \
    clz& operator=(const clz&) = default; \

#define NonCopyCtor(clz) \
    clz(clz&) = delete; \
    clz(const clz&) = delete; \

#define NonCopyAssignOp(clz) \
    clz& operator=(clz&) = delete; \
    clz& operator=(const clz&) = delete; \

#define DefaultMovable(clz) \
    clz(clz&&) noexcept = default; \
    clz& operator=(clz&&) noexcept = default; \

#define NonMovable(clz) \
    clz(clz&&) = delete; \
    clz& operator=(clz&&) = delete; \

#define DefaultCopyable(clz) \
    DefaultCopyCtor(clz) \
    DefaultCopyAssignOp(clz) \

#define NonCopyable(clz) \
    NonCopyCtor(clz) \
    NonCopyAssignOp(clz) \

namespace Common {
    template <uint32_t A, CppIntegral T>
    T AlignUp(T value);

    template <typename LHS, typename... RHS>
    struct IsAllSame {};
}

namespace Common {
    template <uint32_t A, CppIntegral T>
    T AlignUp(T value)
    {
        return (value + (A - 1)) & ~(A - 1);
    }

    template <typename LHS, typename RHS0, typename... RHS>
    struct IsAllSame<LHS, RHS0, RHS...> {
        static constexpr bool value = std::is_same_v<std::remove_cvref_t<LHS>, std::remove_cvref_t<RHS0>> && IsAllSame<LHS, RHS...>::value;
    };

    template <typename LHS, typename RHS>
    struct IsAllSame<LHS, RHS> {
        static constexpr bool value = std::is_same_v<std::remove_cvref_t<LHS>, std::remove_cvref_t<RHS>>;
    };
}
