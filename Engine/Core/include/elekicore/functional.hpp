/// @file functional.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// 関数オブジェクト型エイリアスを提供します

#ifndef ELEKICORE_FUNCTIONAL_HPP
#define ELEKICORE_FUNCTIONAL_HPP

#include <functional>

/// ELEKi ENGINE
namespace ElekiEngine
{

    template<class S> using Func = std::function<S>;                    ///< 関数オブジェクト型エイリアス
    template<class T> using Compare = Func<bool(const T &, const T &)>; ///< 比較関数オブジェクト型エイリアス

    /// 仮名
    namespace Placeholders = std::placeholders;

    /// 関数の引数を束縛します
    template<class...Args>
    auto bind(Args...args)
    {
        return std::bind(args...);
    }

}

#endif // !ELEKICORE_FUNCTIONAL_HPP