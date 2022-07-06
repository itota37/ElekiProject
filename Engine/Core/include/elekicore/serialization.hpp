/// @file serialization.hpp
/// @version 1.22.6
/// @copyright © 2022 Taichi Ito
/// シリアライズ、デシリアライズ機能を提供します
/// 
/// ----- ELEKI BINARY データ構造 -----
/// 
/// {
///     "information" : {
/// 
///         データ
///           :
/// 
///     },
/// 
///     "names" : {
/// 
///         ID名 : ID
/// 
///     },
/// 
///     "instances" : [
/// 
///         {
///     
///             データ
///               :
///             REFERENCE : instance_index,
///               :
/// 
///         },
/// 
///         {
/// 
///             データ
///               :
/// 
///         },
///           :
/// 
///     ]
/// }
/// 
/// -----

#ifndef ELEKICORE_SERIALIZATION_HPP
#define ELEKICORE_SERIALIZATION_HPP

#include <mutex>
#include <thread>
#include "pointer.hpp"
#include "utility.hpp"
#include "tasks.hpp"
#include "map.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{

	/// シリアライザクラスです
	class Serializer;

	/// デシリアライザクラスです
	class Deserializer;

	/// シリアライズ、デシリアライズインタフェースです
	class ISerializable
	{
	public:

		/// シリアライズします
		virtual void serialize(Serializer &serializer) const = 0;

		/// デシリアライズします
		virtual void deserialize(Deserializer &deserializer) = 0;

	};

	/// 非侵入型シリアライズ特殊化クラスです
	template<class T>
	struct Serializable
	{
		/// シリアライズします
		void serialize(Serializer &serializer, const T &value);

		/// デシリアライズします
		void deserialize(Deserializer &deserializer, T &value);
	};

	/// シリアライズ名前空間です
	namespace Serialization
	{

		/// バイナリデータ符号の列挙です
		enum class EBinarySign: u8
		{

			END = 0,        ///< データ構造の終了を表します

			I8 = 1,         ///< i8データ1byteが続きます
			U8 = 2,         ///< u8データ1byteが続きます 
			I16 = 3,        ///< i16データ2byteが続きます
			U16 = 4,        ///< u16データ2byteが続きます
			I32 = 5,        ///< i32データ4byteが続きます
			U32 = 6,        ///< u32データ4byteが続きます
			I64 = 7,        ///< i64データ8byteが続きます
			U64 = 8,        ///< u64データ8byteが続きます
			F32 = 9,        ///< f32データ4byteが続きます
			F64 = 10,       ///< f64データ8byteが続きます
			CHAR = 11,      ///< charデータ1byteが続きます

			TRUE = 32,      ///< trueの値を表現します
			FALSE = 33,     ///< falseの値を表現します
			NIL = 34,       ///< nullptrの値を表現します

			REFERENCE = 64, ///< 32bitID値が続きます
			ARRAY = 65,     ///< ENDまでデータが続きます
			STRUCT = 66,    ///< ENDまでSTRINGとデータのペアが続きます

			STRING = 128,   ///< ENDまでUTF-8文字が続きます
			BINARY = 129,   ///< 32bitサイズ値、サイズ分のbyteデータ、ENDまたはサイズ値と続きます

		};


		//
		// シリアライズ
		// -----

		/// シリアライズ情報構造体です
		struct ELEKICORE_EXPORT SerializeInfo
		{

			Mutex lockFlag;                ///< 排他フラグ
			List<List<u8> *> binaryList;   ///< バイナリデータ
			Map<void *, u32> idMap;        ///< アドレスに対応するIDのマップ
			List<UR<Task<void>>> taskList; ///< タスクリスト
			Map<String, void*> namedAddressMap;

		};

		/// バイナリにシリアライズします
		template<class T>
		struct ToBinary
		{
			void operator()(List<u8> *binary, SerializeInfo &info, const T &value)
			{}
		};

		/// 数値をバイナリに変換します
		template<class T, size_t SIZE = sizeof(T)>
		void numberToBinary(List<u8> *binary, T number)
		{
			auto bytes = (u8 *) (void *) &number;

			switch(endian())
			{
				case EEndian::LITTLE:
				for(size_t i = 0; i < SIZE; i++) binary->add(bytes[i]);
				break;

				case EEndian::BIG:
				for(int i = SIZE - 1; i >= 0; i--) binary->add(bytes[i]);
				break;

				default:
				for(size_t i = 0; i < SIZE; i++) binary->add(0);
				break;
			}
		}

		// 数値をバイナリにシリアライズする特殊化クラスを作成するマクロです
		#define _ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(T, SIGN)          \
		template<> struct ToBinary<T>                                       \
		{                                                                   \
			void operator()(List<u8> *binary, SerializeInfo &info, T value) \
			{                                                               \
				binary->add((u8) SIGN);                                     \
				numberToBinary(binary, value);                              \
			}                                                               \
		}                                                                  
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(i8, EBinarySign::I8);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(u8, EBinarySign::U8);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(i16, EBinarySign::I16);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(u16, EBinarySign::U16);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(i32, EBinarySign::I32);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(u32, EBinarySign::U32);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(i64, EBinarySign::I64);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(u64, EBinarySign::U64);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(f32, EBinarySign::F32);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(f64, EBinarySign::F64);
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(Char, EBinarySign::CHAR);
		
		// 論理値をバイナリにシリアライズします
		template<> struct ToBinary<bool>
		{
			void operator()(List<u8> *binary, SerializeInfo &info, bool value)
			{
				binary->add((u8) (value ? EBinarySign::TRUE : EBinarySign::FALSE));
			}
		};

		// 参照をバイナリにシリアライズします
		template<class T> struct ToBinary<T&>
		{
			void operator()(List<u8> *binary, SerializeInfo &info, const T & value)
			{
				u32 id = 0;                      // 参照先ID
				{
					Lock lock(info.lockFlag);       
					if(info.idMap.contains(&value)) 
					{
						id = info.idMap[&value]; // 登録済みIDを取得
					}
					else
					{
						// バイナリデータバッファの追加位置をIDとして取得
						id = (u32)info.binaryList.count();
						info.idMap.add(&value, id);

						// バイナリデータバッファの追加と登録
						auto newBinary = new(Memory::allocate(List<u8>)) List<u8>();
						info.binaryList.add(newBinary);

						// 参照先のシリアライズを並列に処理
						info.taskList.add(parallel([newBinary, &info, &value]() 
						{ 
							ToBinary<T>{}(newBinary, info, value); 
						}));
					}
				}
				binary->add((u8) EBinarySign::REFERENCE);
				numberToBinary(binary, id);
			}
		};

		// ポインタをバイナリにシリアライズします
		template<class T> struct ToBinary<T *>
		{
			void operator()(List<u8> *binary, SerializeInfo &info, const T *&value)
			{
				if(value)
				{
					ToBinary<T &>{}(binary, info, *value);
				}
				else
				{
					binary->add((u8) EBinarySign::NIL);
				}
			}
		};

		/// 配列をバイナリに変換します
		template<class ITR>
		void arrayToBinary(List<u8> *binary, SerializeInfo &info, ITR begin, ITR end)
		{
			binary->add((u8) EBinarySign::ARRAY);
			for(auto itr = begin; itr != end; itr++)
			{
				ToBinary<decltype(*itr)>{}(binary, info, *itr);
			}
			binary->add((u8) EBinarySign::END);
		}

		// 配列をバイナリにシリアライズする特殊化クラスを作成するマクロです
	    #define _ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(T, A, V, B, E)            \
		template<T> struct ToBinary<A>                                             \
		{                                                                          \
			void operator()(List<u8> *binary, SerializeInfo &info, V)              \
			{                                                                      \
				arrayToBinary(binary, info, B, E);                                 \
			}                                                                      \
		}  
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T COMMA size_t N, T[N], const T(&value)[N], &value[0], &value[N]);
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T, List<T>, const List<T> &value, value.begin(), value.end());
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T, Set<T>, const Set<T> &value, value.begin(), value.end());
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class K COMMA class V, Map<K COMMA V>, const Map<K COMMA V> &value, value.begin(), value.end());

		// KeyValuePairをバイナリにシリアライズします
		template<class K, class V> struct ToBinary<KeyValuePair<K, V>>
		{
			void operator()(List<u8> *binary, SerializeInfo &info, const KeyValuePair<K, V> &value)
			{
				binary->add((u8) EBinarySign::STRUCT);
				ToBinary<String>{}(binary, info, TXT("key"));
				ToBinary<K>{}(binary, info, value.key);
				ToBinary<String>{}(binary, info, TXT("value"));
				ToBinary<V>{}(binary, info, value.value));
				binary->add((u8) EBinarySign::END);
			}
		};

		// ポインタをバイナリにシリアライズします
		template<> struct ToBinary<String>
		{
			void operator()(List<u8> *binary, SerializeInfo &info, const String &value)
			{
				binary->add((u8) EBinarySign::STRING);
				for(size_t i = 0; i < value.count(); i++)
				{
					numberToBinary(binary, value[i]);
				}
				binary->add((u8) EBinarySign::END);
			}
		};

	}

	//
	// 実装
	// -----

	/// シリアライザクラスです
	class ELEKICORE_EXPORT Serializer
	{

	};

	/// デシリアライザクラスです
	class ELEKICORE_EXPORT Deserializer
	{

	};

	template<class T>
	inline void Serializable<T>::serialize(Serializer &serializer, const T &value)
	{}

	template<class T>
	inline void Serializable<T>::deserialize(Deserializer & deserializer, T & value)
	{}

}

#endif // !ELEKICORE_SERIALIZATION_HPP