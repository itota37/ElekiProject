#include <mutex>
#include <string>
#include "elekicore/string.hpp"
#include "elekicore/map.hpp"

using namespace ElekiEngine;

// 文字列情報構造体
struct StringInfo
{
    Char *string;    // 生文字列
    size_t count;    // 生文字列長
    size_t hash;     // 生文字列ハッシュ

    size_t refCount; // この情報を共有しているインスタンス数

    // コンストラクタ
    StringInfo(Char *string, size_t count, size_t hash)
        : string(string)
        , count(count)
        , hash(hash)
        , refCount(0)
    {}
};

// 生文字列ハッシュ関数オブジェクト
struct HashToRawChar
{
    size_t operator()(const Char *string)
    {
        return std::hash<std::string>{}(string);
    }
};

// 生文字列比較関数オブジェクト
struct EqualToRawChar
{
    bool operator()(const Char *l, const Char *r)
    {
        for(size_t i = 0; ; i++)
        {
            if(l[i] == NULL_CHAR && r[i] == NULL_CHAR) return true;
            if(l[i] != r[i]) return false;
            if(l[i] == NULL_CHAR || r[i] == NULL_CHAR) return false;
        }
    }
};

// 文字列情報マップ型
using StringInfoMap = Map<const Char *, StringInfo *, HashToRawChar, EqualToRawChar>;

StringInfoMap *gStringInfos;      // 文字列情報マップ
std::mutex gStringInfoLockFlag;   // 文字列情報マップ排他ロックフラグ
std::once_flag gInitStringInfosF; // initStringInfo初期化フラグ
void initStringInfos()
{
    gStringInfos = new(std::malloc(sizeof(StringInfoMap))) StringInfoMap();
}

// 文字列情報マップから取得します
Char *setStringInfos(const Char *string, size_t &count, size_t &hash)
{
    std::call_once(gInitStringInfosF, initStringInfos);
    Char *str = nullptr;
    if(!gStringInfos->contains(string))
    {
        std::string stdstr(string);
        count = stdstr.size();
        hash = std::hash<std::string>{}(stdstr);
        str = new(Memory::allocate(sizeof(Char) * (stdstr.size() + 1))) Char();
        for(size_t i = 0; i < count + 1; i++) str[i] = string[i];
        auto info = new(Memory::allocate(sizeof(StringInfo))) StringInfo(str, count, hash);
        info->refCount += 1;

        gStringInfos->add(str, info);
    }
    else
    {
        auto info = gStringInfos->at(string);
        str = info->string;
        count = info->count;
        hash = info->hash;
        info->refCount += 1;
    }
    return str;
}

// 文字列情報マップから削除します
void removeStringInfos(const Char *string)
{
    auto info = gStringInfos->at(string);
    gStringInfos->remove(string);
    Memory::deallocate((void *) info->string);
    Memory::deallocate(info);
}

// 文字列を接続します
Char *joinStringInfos(const Char *strL, const Char *strR, size_t &countL, size_t countR, size_t &hashL)
{
    List<Char> tmpstr(countL + countR + 1);
    for(size_t i = 0; i < countL; i++) tmpstr[i] = strL[i];
    for(size_t i = countL, j = 0; j < countR; i++, j++) tmpstr[i] = strR[j];
    tmpstr[countL + countR + 1] = NULL_CHAR;
    std::unique_lock<std::mutex> lock(gStringInfoLockFlag);
    auto retstr = setStringInfos(&tmpstr[0], countL, hashL);
    return retstr;
}

// コンストラクタ
ElekiEngine::String::String()
    : mString(nullptr)
    , mCount(0)
    , mHash(0)
{}

// コンストラクタ
ElekiEngine::String::String(const Char *string)
    : mString(nullptr)
    , mCount(0)
    , mHash(0)
{
    std::unique_lock<std::mutex> lock(gStringInfoLockFlag);
    mString = setStringInfos(string, mCount, mHash);
}

// コピーコンストラクタ
ElekiEngine::String::String(const String &string)
    : mString(nullptr)
    , mCount(0)
    , mHash(0)
{
    std::unique_lock<std::mutex> lock(gStringInfoLockFlag);
    mString = setStringInfos(string.mString, mCount, mHash);
}

// ムーブコンストラクタ
ElekiEngine::String::String(String &&string) noexcept
    : mString(nullptr)
    , mCount(0)
    , mHash(0)
{
    std::unique_lock<std::mutex> lock(gStringInfoLockFlag);
    mString = setStringInfos(string.mString, mCount, mHash);
}

// コピー代入
String &ElekiEngine::String::operator=(const String &string)
{
    auto info = gStringInfos->at(mString);
    std::unique_lock<std::mutex> lock(gStringInfoLockFlag);
    info->refCount -= 1;
    if(!info->refCount)
    {
        removeStringInfos(mString);
    }
    mString = setStringInfos(string.mString, mCount, mHash);
    return *this;
}

// ムーブ代入
String &ElekiEngine::String::operator=(String &&string) noexcept
{
    auto info = gStringInfos->at(mString);
    std::unique_lock<std::mutex> lock(gStringInfoLockFlag);
    info->refCount -= 1;
    if(!info->refCount)
    {
        removeStringInfos(mString);
    }
    mString = setStringInfos(string.mString, mCount, mHash);
    return *this;
}

// 末尾に連結します
String &ElekiEngine::String::operator+=(const String &string)
{
    auto newstr = joinStringInfos(mString, string.mString, mCount, string.mCount, mHash);
    return operator=(newstr);
}

// 末尾に連結します
String &ElekiEngine::String::operator+=(String &&string) noexcept
{
    auto newstr = joinStringInfos(mString, string.mString, mCount, string.mCount, mHash);
    return operator=(newstr);
}

// 文字列が等しいか判定します
bool ElekiEngine::String::operator==(const String &string) const
{
    return mString == string.mString;
}

// 文字列が等しくないか判定します
bool ElekiEngine::String::operator!=(const String &string) const
{
    return mString != string.mString;
}

// 添え字から文字を取得します
const Char &ElekiEngine::String::operator[](size_t index) const
{
    if(index >= mCount) printError("out of range. Char String::operator[](size_t index) const");
    return mString[index];
}

// 文字列長を返します
size_t ElekiEngine::String::count() const
{
    return mCount;
}

/// 生文字列を返します
const Char *ElekiEngine::String::cstr() const
{
    return mString;
}

// 末尾に連結します
String &ElekiEngine::String::join(const String &string)
{
    return operator+=(string);
}

// 末尾に連結します
String &ElekiEngine::String::join(String &&string) noexcept
{
    return operator+=(string);
}

// 添え字から文字を取得します
const Char &ElekiEngine::String::at(size_t index) const
{
    if(index >= mCount) printError("out of range. Char String::at(size_t index) const");
    return mString[index];
}

// 文字列の一部を抜き出します
// @param index 抜き出す文字列の先頭位置
// @param length 抜き出す文字列の長さ
String ElekiEngine::String::sub(size_t index, size_t length) const
{
    if(index + length >= mCount) printError("out of range. String String::sub(size_t index, size_t length) const");
    List<Char> str(length + 1);
    for(size_t i = 0, j = index; i < length; i++, j++) str[i] = mString[j];
    str[length] = NULL_CHAR;
    return &str[0];
}

// 文字列の指定位置から後ろを抜き出します
// @param index 抜き出す文字列の先頭位置
// @param invers trueのとき指定位置より前を抜き出します
String ElekiEngine::String::sub(size_t index, bool invers) const
{
    if(index >= mCount) printError("out of range. String String::sub(size_t index, bool invers) const");
    
    if(!invers)
    {
        return sub(index, mCount - index);
    }
    else
    {
        return sub(0, index);
    }
}

// 一致する文字の位置を返します
// @param ch 検索する文字
// @param invers 真のとき末尾から、偽のとき先頭から走査します
// @return 一致した文字の位置
// @retval 要素数 一致する文字がありませんでした
size_t ElekiEngine::String::indexOf(Char ch, bool invers) const
{
    if(!invers)
    {
        for(size_t i = 0; i < mCount; i++)
        {
            if(ch == mString[i]) return i;
        }
    }
    else
    {
        for(int i = mCount - 1; i >= 0; i--)
        {
            if(ch == mString[i]) return i;
        }
    }
    return mCount;
}

// 一致する文字列の位置を返します
// @param string 検索する文字
// @param invers 真のとき末尾から、偽のとき先頭から走査します
// @return 一致した文字列の位置
// @retval 要素数 一致する文字列がありませんでした
size_t ElekiEngine::String::indexOf(const String &string, bool invers) const
{
    if(mCount > string.mCount) return mCount;

    if(!invers)
    {
        for(size_t i = 0; i < mCount; i++)
        {
            for(size_t j = i, k = 0; j < mCount; j++, k++)
            {
                if(mString[j] != string.mString[k]) break;
                if(k == string.mCount - 1) return i;
            }
        }
    }
    else
    {
        for(int i = mCount - string.mCount - 1; i >= 0; i--)
        {
            for(size_t j = i, k = 0; j < mCount; j++, k++)
            {
                if(mString[j] != string.mString[k]) break;
                if(k == string.mCount - 1) return i;
            }
        }
    }
    return mCount;
}

// イテレータの先頭を返します
PointerItr<const Char &> ElekiEngine::String::begin() const
{
    return PointerItr<const Char &>(&mString[0]);
}

// イテレータの番兵を返します
PointerItr<const Char &> ElekiEngine::String::end() const
{
    return PointerItr<const Char &>(&mString[mCount]);
}

// 文字列に変換します
String ElekiEngine::i8ToString(const i8 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::u8ToString(const u8 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::i16ToString(const i16 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::u16ToString(const u16 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::i32ToString(const i32 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::u32ToString(const u32 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::i64ToString(const i64 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::u64ToString(const u64 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::f32ToString(const f32 &value)
{
    return std::to_string(value).c_str();
}

// 文字列に変換します
String ElekiEngine::f64ToString(const f64 &value)
{
    return std::to_string(value).c_str();
}

// 文字列から値に変換します
// @param string 文字列
// @param [out] value 変換後の値
// @return 変換に成功した場合真
bool ElekiEngine::stringToI32(const String &string, i32 &value)
{
    try
    {
        value = std::stoi(string.cstr());
        return true;
    }
    catch(...)
    {
        return false;
    }
}

// 文字列から値に変換します
// @param string 文字列
// @param [out] value 変換後の値
// @return 変換に成功した場合真
bool ElekiEngine::stringToU32(const String &string, u32 &value)
{
    try
    {
        value = (u32)std::stoul(string.cstr());
        return true;
    }
    catch(...)
    {
        return false;
    }
}

// 文字列から値に変換します
// @param string 文字列
// @param [out] value 変換後の値
// @return 変換に成功した場合真
bool ElekiEngine::stringToI64(const String &string, i64 &value)
{
    try
    {
        value = std::stoll(string.cstr());
        return true;
    }
    catch(...)
    {
        return false;
    }
}

// 文字列から値に変換します
// @param string 文字列
// @param [out] value 変換後の値
// @return 変換に成功した場合真
bool ElekiEngine::stringToU64(const String &string, u64 &value)
{
    try
    {
        value = std::stoull(string.cstr());
        return true;
    }
    catch(...)
    {
        return false;
    }
}

// 文字列から値に変換します
// @param string 文字列
// @param [out] value 変換後の値
// @return 変換に成功した場合真
bool ElekiEngine::stringToF32(const String &string, f32 &value)
{
    try
    {
        value = std::stof(string.cstr());
        return true;
    }
    catch(...)
    {
        return false;
    }
}

// 文字列から値に変換します
// @param string 文字列
// @param [out] value 変換後の値
// @return 変換に成功した場合真
bool ElekiEngine::stringToF64(const String &string, f64 &value)
{
    try
    {
        value = std::stod(string.cstr());
        return true;
    }
    catch(...)
    {
        return false;
    }
}