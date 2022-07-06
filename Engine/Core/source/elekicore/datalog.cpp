#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif
#include "elekicore/datalog.hpp"
#include "elekicore/allocation.hpp"

using namespace ElekiEngine;

// 文字列を変換して関数ポインタに渡して実行します
void utf8ToChar(const Char *string, void(*printFunc)(const char *))
{
    auto u8str = (const char *) string;

#if defined(_WIN32) || defined(_WIN64)

    auto u8len = strlen(u8str);

    // UTF-8 -> UTF-16
    auto u16len = MultiByteToWideChar(CP_UTF8, 0, u8str, (int) u8len, nullptr, 0);
    auto u16str = (wchar_t *) Memory::allocate(sizeof(wchar_t) * u16len);
    auto res = MultiByteToWideChar(CP_UTF8, 0, u8str, u8len, u16str, u16len);

    //UTF-16 -> Shift-JIS
    auto clen = WideCharToMultiByte(CP_ACP, 0, u16str, (int) u16len, nullptr, 0, nullptr, nullptr);
    auto cstr = (char *) Memory::allocate(sizeof(char) * u16len);
    res = ::WideCharToMultiByte(CP_ACP, 0, u16str, (int) u16len, cstr, (int) clen, nullptr, nullptr);

    Memory::deallocate(u16str);
    printFunc(cstr);
    Memory::deallocate(cstr);

#else

    printFunc(cstr);

#endif
}

// 標準出力へ出力します
void printLog(const char *string)
{
    std::cout << string << std::endl;
}

// エラー出力へ出力します
void printErr(const char *string)
{
    std::cerr << string << std::endl;
}

// 文字列をログへ出力します
// @param string 出力する標準文字列
void ElekiEngine::print(const Char *string)
{
    utf8ToChar(string, &printLog);
}

// 文字列をエラーログへ出力します
// @param string 出力する標準文字列
void ElekiEngine::printError(const Char *string)
{
    utf8ToChar(string, &printErr);
}