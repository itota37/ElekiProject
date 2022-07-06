/// @file integer.hpp
/// @version 1.22.4
/// @copyright © 2022 Taichi Ito
/// 整数型エイリアス、定数を提供します

#ifndef ELEKICORE_INTEGER_HPP
#define ELEKICORE_INTEGER_HPP

#include <limits>

/// ELEKi ENGINE
namespace ElekiEngine
{
    
    using i8 = signed char;         ///< 符号付き8ビット整数型エイリアス
    using u8 = unsigned char;       ///< 符号無し8ビット整数型エイリアス
    using i16 = signed short;       ///< 符号付き16ビット整数型エイリアス
    using u16 = unsigned short;     ///< 符号無し16ビット整数型エイリアス
    using i32 = signed int;         ///< 符号付き32ビット整数型エイリアス
    using u32 = unsigned int;       ///< 符号無し32ビット整数型エイリアス
    using i64 = signed long long;   ///< 符号付き64ビット整数型エイリアス
    using u64 = unsigned long long; ///< 符号無し64ビット整数型エイリアス

    constexpr i8 I8_MIN = std::numeric_limits<i8>::min();    ///< 型の最小値
    constexpr u8 U8_MIN = std::numeric_limits<u8>::min();    ///< 型の最小値
    constexpr u8 U8_MAX = std::numeric_limits<u8>::max();    ///< 型の最大値
    constexpr i16 I16_MIN = std::numeric_limits<i16>::min(); ///< 型の最小値
    constexpr u16 U16_MIN = std::numeric_limits<u16>::min(); ///< 型の最小値
    constexpr u16 U16_MAX = std::numeric_limits<u16>::max(); ///< 型の最大値
    constexpr i32 I32_MIN = std::numeric_limits<i32>::min(); ///< 型の最小値
    constexpr u32 U32_MIN = std::numeric_limits<u32>::min(); ///< 型の最小値
    constexpr u32 U32_MAX = std::numeric_limits<u32>::max(); ///< 型の最大値
    constexpr i64 I64_MIN = std::numeric_limits<i64>::min(); ///< 型の最小値
    constexpr u64 U64_MIN = std::numeric_limits<u64>::min(); ///< 型の最小値
    constexpr u64 U64_MAX = std::numeric_limits<u64>::max(); ///< 型の最大値

}

#endif // !ELEKICORE_INTEGER_HPP