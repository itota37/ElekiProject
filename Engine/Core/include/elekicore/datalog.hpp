/// @file datalog.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// データログ出力機能を提供します

#ifndef ELEKICORE_DATALOG_HPP
#define ELEKICORE_DATALOG_HPP

#include "preprocess.hpp"

/// 標準生文字列にエンコードします
#define TXT(T) u8##T

/// ELEKi ENGINE
namespace ElekiEngine
{
    using Char = char; ///< 標準文字型

    constexpr Char NULL_CHAR = '\0'; ///< ヌル文字

    /// 文字列をログへ出力します
    /// @param string 出力する標準文字列
    void ELEKICORE_EXPORT print(const Char *string);

    /// 文字列をエラーログへ出力します
    /// @param string 出力する標準文字列
    void ELEKICORE_EXPORT printError(const Char *string);

}

#endif // !ELEKICORE_DATALOG_HPP