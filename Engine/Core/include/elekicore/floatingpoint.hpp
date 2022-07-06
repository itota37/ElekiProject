/// @file floatingpoint.hpp
/// @version 1.22.4
/// @copyright © 2022 Taichi Ito
/// 浮動小数点数型エイリアス、定数を提供します

#ifndef ELEKICORE_FLOATINGPOINT_HPP
#define ELEKICORE_FLOATINGPOINT_HPP

#include <limits>

/// ELEKi ENGINE
namespace ElekiEngine
{

    using f32 = float;  ///< 32ビット浮動小数点数型エイリアス
    using f64 = double; ///< 64ビット浮動小数点数型エイリアス

    constexpr f32 F32_MIN = std::numeric_limits<f32>::min();                 ///< 型の最小値
    constexpr f32 F32_MAX = std::numeric_limits<f32>::max();                 ///< 型の最大値
    constexpr f32 F32_EPSILON = std::numeric_limits<f32>::epsilon();         ///< 誤差値
    constexpr f32 F32_INFINITY = std::numeric_limits<f32>::infinity();       ///< 無限大値
    constexpr f32 F32_NEGA_INFINITY = -std::numeric_limits<f32>::infinity(); ///< 負の無限大値
    constexpr f64 F64_MIN = std::numeric_limits<f64>::min();                 ///< 型の最小値
    constexpr f64 F64_MAX = std::numeric_limits<f64>::max();                 ///< 型の最大値
    constexpr f64 F64_EPSILON = std::numeric_limits<f64>::epsilon();         ///< 誤差値
    constexpr f64 F64_INFINITY = std::numeric_limits<f64>::infinity();       ///< 無限大値
    constexpr f64 F64_NEGA_INFINITY = -std::numeric_limits<f64>::infinity(); ///< 負の無限大値

}

#endif // !ELEKICORE_FLOATINGPOINT_HPP