#include <functional>
#include "elekicore/hash.hpp"

/// ハッシュ値を返します
size_t ElekiEngine::i8ToHash(const i8 &value)
{
	return std::hash<i8>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::u8ToHash(const u8 &value)
{
	return std::hash<u8>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::i16ToHash(const i16 &value)
{
	return std::hash<i16>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::u16ToHash(const u16 &value)
{
	return std::hash<u16>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::i32ToHash(const i32 &value)
{
	return std::hash<i32>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::u32ToHash(const u32 &value)
{
	return std::hash<u32>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::i64ToHash(const i64 &value)
{
	return std::hash<i64>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::u64ToHash(const u64 &value)
{
	return std::hash<u64>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::f32ToHash(const f32 &value)
{
	return std::hash<f32>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::f64ToHash(const f64 &value)
{
	return std::hash<f64>{}(value);
}

/// ハッシュ値を返します
size_t ElekiEngine::boolToHash(const bool &value)
{
	return std::hash<bool>{}(value);
}