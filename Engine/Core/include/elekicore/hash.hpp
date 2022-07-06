/// @file hash.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// ハッシュ生成特殊化クラス、生成関数を提供します

#ifndef ELEKICORE_HASH_HPP
#define ELEKICORE_HASH_HPP

#include "preprocess.hpp"
#include "integer.hpp"
#include "floatingpoint.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{

    /// ハッシュ特殊化構造体です
    template<class T>
    struct Hash
    {
        /// ハッシュ値を返します
        size_t operator()(const T &value)
        {
            static_assert(false, "not supported type. Hash<T>::operator()(const T &value)");
            return 0;
        }
    };

    /// ハッシュ値を返します
    /// @param value ハッシュ値を生成する値
    template<class T>
    size_t toHash(const T &value)
    {
        return Hash<T>{}(value);
    }

    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT i8ToHash(const i8 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT u8ToHash(const u8 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT i16ToHash(const i16 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT u16ToHash(const u16 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT i32ToHash(const i32 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT u32ToHash(const u32 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT i64ToHash(const i64 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT u64ToHash(const u64 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT f32ToHash(const f32 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT f64ToHash(const f64 &value);
    /// ハッシュ値を返します
    size_t ELEKICORE_EXPORT boolToHash(const bool &value);

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<i8>
    {
        /// ハッシュ値を返します
        size_t operator()(const i8 &value)
        {
            return i8ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<u8>
    {
        /// ハッシュ値を返します
        size_t operator()(const u8 &value)
        {
            return u8ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<i16>
    {
        /// ハッシュ値を返します
        size_t operator()(const i16 &value)
        {
            return i16ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<u16>
    {
        /// ハッシュ値を返します
        size_t operator()(const u16 &value)
        {
            return u16ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<i32>
    {
        /// ハッシュ値を返します
        size_t operator()(const i32 &value)
        {
            return i32ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<u32>
    {
        /// ハッシュ値を返します
        size_t operator()(const u32 &value)
        {
            return u32ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<i64>
    {
        /// ハッシュ値を返します
        size_t operator()(const i64 &value)
        {
            return i64ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<u64>
    {
        /// ハッシュ値を返します
        size_t operator()(const u64 &value)
        {
            return u64ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<f32>
    {
        /// ハッシュ値を返します
        size_t operator()(const f32 &value)
        {
            return f32ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<f64>
    {
        /// ハッシュ値を返します
        size_t operator()(const f64 &value)
        {
            return f64ToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<bool>
    {
        /// ハッシュ値を返します
        size_t operator()(const bool &value)
        {
            return boolToHash(value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<char>
    {
        /// ハッシュ値を返します
        size_t operator()(const char &value)
        {
            return u8ToHash((u8)value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<wchar_t>
    {
        /// ハッシュ値を返します
        size_t operator()(const wchar_t &value)
        {
            return u32ToHash((u32)value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<char16_t>
    {
        /// ハッシュ値を返します
        size_t operator()(const char16_t &value)
        {
            return u16ToHash((u16)value);
        }
    };

    /// ハッシュ特殊化構造体です
    template<>
    struct Hash<char32_t>
    {
        /// ハッシュ値を返します
        size_t operator()(const char32_t &value)
        {
            return u32ToHash((u32)value);
        }
    };
}

#endif // !ELEKICORE_HASH_HPP