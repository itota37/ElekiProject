/// @file string.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// 文字列を提供します

#ifndef ELEKICORE_STRING_HPP
#define ELEKICORE_STRING_HPP

#include "hash.hpp"
#include "array.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{

    /// 標準文字列クラスです
    class ELEKICORE_EXPORT String
    {
        template<class T> friend struct Hash;

        Char *mString; // 生文字列
        size_t mCount; // 生文字列長
        size_t mHash;  // 生文字列ハッシュ

    public:

        /// コンストラクタ
        String();

        /// コンストラクタ
        String(const Char *string);

        /// コピーコンストラクタ
        String(const String &string);

        /// ムーブコンストラクタ
        String(String &&string) noexcept;

        /// コピー代入
        String &operator=(const String &string);

        /// ムーブ代入
        String &operator=(String &&string) noexcept;

        /// 末尾に連結します
        String &operator+=(const String &string);

        /// 末尾に連結します
        String &operator+=(String &&string) noexcept;

        /// 文字列が等しいか判定します
        bool operator==(const String &string) const;

        /// 文字列が等しくないか判定します
        bool operator!=(const String &string) const;

        /// 添え字から文字を取得します
        const Char &operator[](size_t index) const;

        /// 文字列長を返します
        size_t count() const;

        /// 生文字列を返します
        const Char *cstr() const;

        /// 末尾に連結します
        String &join(const String &string);

        /// 末尾に連結します
        String &join(String &&string) noexcept;

        /// 添え字から文字を取得します
        const Char &at(size_t index) const;

        /// 文字列の一部を抜き出します
        /// @param index 抜き出す文字列の先頭位置
        /// @param length 抜き出す文字列の長さ
        String sub(size_t index, size_t length) const;

        /// 文字列の指定位置から後ろを抜き出します
        /// @param index 抜き出す文字列の先頭位置
        /// @param invers trueのとき指定位置より前を抜き出します
        String sub(size_t index, bool invers = false) const;

        /// 一致する文字の位置を返します
        /// @param ch 検索する文字
        /// @param invers 真のとき末尾から、偽のとき先頭から走査します
        /// @return 一致した文字の位置
        /// @retval 要素数 一致する文字がありませんでした
        size_t indexOf(Char ch, bool invers = false) const;

        /// 一致する文字列の位置を返します
        /// @param string 検索する文字
        /// @param invers 真のとき末尾から、偽のとき先頭から走査します
        /// @return 一致した文字列の位置
        /// @retval 要素数 一致する文字列がありませんでした
        size_t indexOf(const String &string, bool invers = false) const;

        /// '添え字'の位置の引数を文字列の'{添え字}'の位置に埋め込みます
        /// @param values 埋め込む値
        template<class...Ts>
        String format(Ts...values) const;

        /// イテレータの先頭を返します
        PointerItr<const Char &> begin() const;

        /// イテレータの番兵を返します
        PointerItr<const Char &> end() const;

    };

    /// ハッシュ特殊化クラスです
    template<>
    struct Hash<String>
    {
        /// ハッシュ値を返します
        size_t operator()(const String &value)
        {
            return value.mHash;
        }
    };

    /// 文字列変換特殊化クラスです
    template<class T>
    struct ToString
    {
        /// 文字列に変換します
        String operator()(const T &value)
        {
            static_assert(false, "not supported type. String ToString<T>::operator()(const T &value)");
            return T("");
        }
    };

    /// 文字列に変換します
    template<class T>
    String toString(const T &value)
    {
        return ToString<T>{}(value);
    }

    /// 文字列に変換します
    String ELEKICORE_EXPORT i8ToString(const i8 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT u8ToString(const u8 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT i16ToString(const i16 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT u16ToString(const u16 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT i32ToString(const i32 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT u32ToString(const u32 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT i64ToString(const i64 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT u64ToString(const u64 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT f32ToString(const f32 &value);
    /// 文字列に変換します
    String ELEKICORE_EXPORT f64ToString(const f64 &value);

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<i8>
    {
        /// 文字列に変換します
        String operator()(const i8 &value)
        {
            return i8ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<u8>
    {
        /// 文字列に変換します
        String operator()(const u8 &value)
        {
            return u8ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<i16>
    {
        /// 文字列に変換します
        String operator()(const i16 &value)
        {
            return i16ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<u16>
    {
        /// 文字列に変換します
        String operator()(const u16 &value)
        {
            return u16ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<i32>
    {
        /// 文字列に変換します
        String operator()(const i32 &value)
        {
            return i32ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<u32>
    {
        /// 文字列に変換します
        String operator()(const u32 &value)
        {
            return u32ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<i64>
    {
        /// 文字列に変換します
        String operator()(const i64 &value)
        {
            return i64ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<u64>
    {
        /// 文字列に変換します
        String operator()(const u64 &value)
        {
            return u64ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<f32>
    {
        /// 文字列に変換します
        String operator()(const f32 &value)
        {
            return f32ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<f64>
    {
        /// 文字列に変換します
        String operator()(const f64 &value)
        {
            return f64ToString(value);
        }
    };

    /// 文字列変換特殊化クラスです
    template<>
    struct ToString<bool>
    {
        /// 文字列に変換します
        String operator()(const bool &value)
        {
            return (value ? TXT("true") : TXT("false"));
        }
    };

    /// 文字列から値に変換します
    /// @param string 文字列
    /// @param [out] value 変換後の値
    /// @return 変換に成功した場合真
    bool ELEKICORE_EXPORT stringToI32(const String &string, i32 &value);

    /// 文字列から値に変換します
    /// @param string 文字列
    /// @param [out] value 変換後の値
    /// @return 変換に成功した場合真
    bool ELEKICORE_EXPORT stringToU32(const String &string, u32 &value);

    /// 文字列から値に変換します
    /// @param string 文字列
    /// @param [out] value 変換後の値
    /// @return 変換に成功した場合真
    bool ELEKICORE_EXPORT stringToI64(const String &string, i64 &value);

    /// 文字列から値に変換します
    /// @param string 文字列
    /// @param [out] value 変換後の値
    /// @return 変換に成功した場合真
    bool ELEKICORE_EXPORT stringToU64(const String &string, u64 &value);

    /// 文字列から値に変換します
    /// @param string 文字列
    /// @param [out] value 変換後の値
    /// @return 変換に成功した場合真
    bool ELEKICORE_EXPORT stringToF32(const String &string, f32 &value);

    /// 文字列から値に変換します
    /// @param string 文字列
    /// @param [out] value 変換後の値
    /// @return 変換に成功した場合真
    bool ELEKICORE_EXPORT stringToF64(const String &string, f64 &value);

    // 引数の文字列リストを作成するクラス
    template<class T, class...Ts>
    struct _MakeStringList
    {
        void operator()(List<String> &strings, const T &value, const T &...values)
        {
            strings.add(toString(value));
            _MakeStringList<Ts...>{}(strings, values...);
        }
    };

    // 引数の文字列リストを作成するクラスの末尾の特殊化
    template<class T>
    struct _MakeStringList<T>
    {
        void operator()(List<String> &strings, const T &value)
        {
            strings.add(toString(value));
        }
    };

    // '添え字'の位置の引数を文字列の'{添え字}'の位置に埋め込みます
    // @param values 埋め込む値
    template<class ...Ts>
    String ElekiEngine::String::format(Ts ...values) const
    {

        // 引数から文字列リストを作成
        List<String> valstrs;
        _MakeStringList<Ts...>{}(valstrs, values...);

        // 戻り値用の文字リスト
        List<Char> retstr;

        // 文字列を埋め込む
        for(size_t i = 0; i < mCount; i++)
        {

            // { から } までの処理
            if('{' == mString[i])
            {
                List<Char> numstr;
                for(size_t j = i + 1; j < mCount; j++)
                {
                    if('}' == mString[j])
                    {
                        u32 index = 0;
                        numstr.add(NULL_CHAR);
                        // {} 、変換失敗、または、添え字が範囲外の場合
                        if(!numstr.count() ||
                           !stringToU32(&numstr[0], index) ||
                           valstrs.count() >= index)
                        {
                            // { から先も文字として挿入
                            retstr.add(mString[i]);
                            break;
                        }
                        // 値を挿入
                        for(size_t i = 0; i < valstrs[index].count(); i++)
                        {
                            retstr.add(valstrs[index][i]);
                        }
                        // { 型 } までの区間をとばす
                        i = j;
                    }
                    numstr.add(mString[j]);
                }
            }
            // { から } までの区間ではないので挿入
            else
            {
                retstr.add(mString[i]);
            }
        }
        retstr.add(NULL_CHAR);
        return &retstr[0];
    }
}

#endif // !ELEKICORE_STRING_HPP