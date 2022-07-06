/// @file preprocess.hpp
/// @version 1.22.4
/// @copyright © 2022 Taichi Ito
/// シンボル、及び、マクロを提供します

#ifndef ELEKICORE_PREPROCESS_HPP
#define ELEKICORE_PREPROCESS_HPP

//
// 汎用マクロ
// -----

#define COMMA , ///< カンマ

//
// 静的エラーマクロ
// -----


/// このコンパイラはサポートされていません
#define ELEKI_STATICERROR_NOT_SUPPORTED_COMPILER static_assert(false, "not supported compiler.");


/// このOSはサポートされていません
#define ELEKI_STATICERROR_NOT_SUPPORTED_OS static_assert(false, "not supported os.");


//
// コンパイラを判別します
// -----


#if defined(_MSC_VER)


#define ELEKI_COMPILER_VC 1    ///< VisualC++


#elif defined(__clang__)


#define ELEKI_COMPILER_CLANG 1 ///< Clang


#if defined(__APPLE__)


#define ELEKI_COMPILER_APPLE 1 ///< Clang/Apple


#endif


#elif defined(__GNUC__)


#define ELEKI_COMPILER_GCC 1   ///< GCC


#else


ELEKI_STATICERROR_NOT_SUPPORTED_COMPILER


#endif


//
// OSを判別します
// -----


#if defined(_WIN64)


#define ELEKI_OS_WINDOWS64 1 ///< Windows64
#define ELEKI_OS_WINDOWS   1 ///< Windows


#elif defined(_WIN32)


#define ELEKI_OS_WINDOWS32 1 ///< Windows64
#define ELEKI_OS_WINDOWS   1 ///< Windows


#elif defined(__linux__)


#define ELEKI_OS_LINUX 1     ///< Linux


#if defined(__x86_64)


#define ELEKI_OS_LINUX64 1  ///< Linux/64bit


#endif


#elif defined(__unix__)


#define ELEKI_OS_UNIX 1      ///< Unix


#if defined(__MUCH__)


#define ELEKI_OS_MAC 1      ///< macOS


#if defined(__x86_64)


#define ELEKI_OS_MAC64 1    ///< macOS/64bit


#endif


#elif defined(__FreeBSD__)


#define ELEKI_OS_FREEBSD 1    ///< FreeBSD


#endif 



#endif


//
// コンパイラ間での公開方法の差を埋めます
// -----

#if ELEKI_COMPILER_VC

/// 外部に公開します
#define EXPORT __declspec(dllexport)

/// 外部から読込ます
#define IMPORT __declspec(dllimport)

#else

/// 外部に公開します
#define EXPORT

/// 外部から読込ます
#define IMPORT

#endif

//
// モジュール間での公開、読込を1つのコードで可能にします
// -----

// ELEKi Core モジュールをコンパイルする場合、ELEKICOREを1で宣言しなくてはなりません
#if ELEKICORE

/// ELEKi Core モジュールで公開されます
#define ELEKICORE_EXPORT EXPORT

#else

/// ELEKi Core モジュールで公開されます
#define ELEKICORE_EXPORT IMPORT

#endif // ELEKICORE


#endif // !ELEKICORE_PREPROCESS_HPP