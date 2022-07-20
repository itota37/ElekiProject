/// @file serialization.hpp
/// @version 1.22.6
/// @copyright © 2022 Taichi Ito
/// シリアライズ、デシリアライズ機能を提供します
/// 
/// ElekiBinary構成
/// -----
/// 
/// <ヘッダー部><データ部>から構成されます
/// 
/// ヘッダー部
/// 00 - 10 "ELEKIBINARY" ASC2文字列
/// 11 - 15 符号なし4バイト整数、バージョン情報
/// 
/// データ部
/// <サイズ><インスタンスデータ>から構成されるデータの配列です
/// サイズは符号なし4バイト整数で表現されます
/// 
/// インスタンスデータ
/// <サイン><データ>から構成され、サインからデータの構造、値を判別します
/// 
/// I8サイン        サインの後、符号付き8ビット整数が続きます
/// U8サイン        サインの後、符号無し8ビット整数が続きます
/// I16サイン       サインの後、符号付き16ビット整数が続きます
/// U16サイン       サインの後、符号無し16ビット整数が続きます
/// I32サイン       サインの後、符号付き32ビット整数が続きます
/// U32サイン       サインの後、符号無し32ビット整数が続きます
/// I64サイン       サインの後、符号付き64ビット整数が続きます
/// U64サイン       サインの後、符号無し64ビット整数が続きます
/// TRUEサイン      論理値trueを表す
/// FALSEサイン     論理値falseを表す
/// NILサイン       nullptrを表す
/// REFERENCEサイン U32データで参照先データのデータ配列の添え字、または、STRINGデータで外部参照名が続きます
/// ARRAYサイン	    ENDサインまでデータが続きます
/// STRUCTサイン    ENDサインまでSTRINGデータとデータの組が続きます
/// STRINGサイン    ENDサインまでUTF-8文字列が続きます
/// BINARYサイン    サインの後、U32データでサイズが記録され、サイズ分バイナリデータが続きます
///                 バイナリデータの後にBINARYサインが続く場合さらにデータが続き、ENDサインの場合終了します
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
	/// バイナリデータクラスです
	class ELEKICORE_EXPORT Binary: public List<u8>
	{
	public:
		/// コンストラクタ
		/// @param count 初期要素数
		/// @param allocator 使用するアロケータ
		Binary(size_t count, IAllocator *allocator = Memory::allocator());

		/// コンストラクタ
		/// @param allocator 使用するアロケータ
		Binary(IAllocator *allocator = Memory::allocator());

		/// コンストラクタ
		/// @param list 初期化リスト
		/// @param allocator 使用するアロケータ
		Binary(std::initializer_list<u8> list, IAllocator *allocator = Memory::allocator());

		/// コピーコンストラクタ
		Binary(const Binary &binary);

		/// ムーブコンストラクタ
		Binary(Binary &&binary) noexcept;

		/// デストラクタ
		~Binary();

		/// コピー代入します
		Binary &operator=(const Binary &binary);

		/// ムーブ代入します
		Binary &operator=(Binary &&binary) noexcept;

		/// 末尾に追加します
		Binary &operator+=(const Binary &binary);

		/// 末尾に追加します
		Binary &operator+=(Binary &&binary) noexcept;

		/// 末尾に追加します
		Binary &operator+=(const u8 &element);

		/// 末尾に追加します
		Binary &operator+=(u8 &&element) noexcept;

		/// 添え字から要素にアクセスします
		u8 &operator[](size_t index);

		/// 添え字から要素にアクセスします
		const u8 &operator[](size_t index) const;

	};

	/// 連結します
	Binary operator+(const Binary &l, const Binary &r);

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
	
	/// シリアライズ可能アロケータインタフェースです
	class ISerializableAllocator: public IAllocator, public ISerializable {};

	/// シリアライズ名前空間です
	namespace Serialization
	{
		/// バイナリ情報です
		struct ELEKICORE_EXPORT BinaryInformation
		{
			/// データ形式識別名
			static const String NAME;

			/// シリアライズバージョンです
			static constexpr u32 VERSION = 1220701;

			/// 情報サイズ
			static constexpr size_t SIZE = 16;
		};

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

			TRUE = 32,      ///< trueの値を表現します
			FALSE = 33,     ///< falseの値を表現します
			NIL = 34,       ///< nullptrの値を表現します

			REFERENCE = 64, ///< U32データで参照先データのデータ配列の添え字、または、STRINGデータで外部参照名が続きます
			ARRAY = 65,     ///< ENDまでデータが続きます
			STRUCT = 66,    ///< ENDまでSTRINGとデータの組が続きます

			STRING = 128,   ///< ENDまでUTF-8文字が続きます
			BINARY = 129,   ///< 32bitサイズ値、サイズ分のbyteデータ、ENDまたはサイズ値と続きます

		};


		//
		// シリアライズ
		// -----

		/// シリアライズ情報構造体です
		struct ELEKICORE_EXPORT SerializeInfo
		{

			Mutex lockFlag;                 ///< 排他フラグ
			List<UR<List<u8>>> binaryList;    ///< バイナリデータ
			Map<void *, u32> indexMap;      ///< 参照先がシリアライズされた位置のマップ
			List<UR<Task<void>>> taskList;  ///< タスクリスト
			Map<void *, String> ptrNameMap; ///< 名前付き外部ポインタのマップ

		};

		/// バイナリにシリアライズします
		template<class T>
		struct ToBinary
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const T &value);
		};

		/// 数値をバイナリに変換します
		template<class T, size_t SIZE = sizeof(T)>
		void numberToBinary(Ref<List<u8>> binary, T number)
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
		#define _ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(T, SIGN)              \
		template<> struct ToBinary<T>                                           \
		{                                                                       \
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, T value) \
			{                                                                   \
				binary->add((u8) SIGN);                                         \
				numberToBinary(binary, value);                                  \
			}                                                                   \
		};                                                                  
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
		_ELEKICORE_SERIALIZATION_NUMBER_TO_BINARY(Char, EBinarySign::U8);
		
		// 論理値をバイナリにシリアライズします
		template<> struct ToBinary<bool>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, bool value)
			{
				binary->add((u8) (value ? EBinarySign::TRUE : EBinarySign::FALSE));
			}
		};

		// 参照をバイナリにシリアライズします
		template<class T> struct ToBinary<T&>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const T & value)
			{
				static const String EMPTY_STRING = TXT("");

				u32 index = 0;                   // 参照先リストのインデクス
				String name = EMPTY_STRING;      // 名前付きポインタ名
				{
					Lock lock(info.lockFlag);  

					if(info.ptrNameMap.contains(&value))
					{
						name = info.ptrNameMap[&value]; // 名前付きポインタの名前を取得
					}
					else if(info.indexMap.contains(&value)) 
					{
						index = info.indexMap[&value];  // 登録済みIDを取得
					}
					else
					{
						// バイナリデータバッファの追加位置をIDとして取得
						index = (u32)info.binaryList.count();
						info.indexMap.add(&value, index);

						// バイナリデータバッファの追加と登録
						auto urBinary = newUR<List<u8>>();
						Ref<List<u8>> refBinary = urBinary;
						info.binaryList.add(urBinary);

						// 参照先のシリアライズを並列に処理
						info.taskList.add(parallel([refBinary, &info, &value]() 
						{ 
							ToBinary<T>{}(refBinary, info, value); 
						}));
					}
				}
				binary->add((u8) EBinarySign::REFERENCE);
				if(name != EMPTY_STRING)
				{
					// ポインタ名でシリアライズ
					ToBinary<String>{}(binary, info, name);
				}
				else
				{
					// 参照先インデクスでシリアライズ
					ToBinary<u32>{}(binary, info, index);
				}
			}
		};

		// ポインタをバイナリにシリアライズします
		template<class T> struct ToBinary<T *>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const T *&value)
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
		void arrayToBinary(Ref<List<u8>> binary, SerializeInfo &info, ITR begin, ITR end)
		{
			binary->add((u8) EBinarySign::ARRAY);
			for(auto itr = begin; itr != end; itr++)
			{
				ToBinary<decltype(*itr)>{}(binary, info, *itr);
			}
			binary->add((u8) EBinarySign::END);
		}

		// 配列をバイナリにシリアライズする特殊化クラスを作成するマクロです
	    #define _ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(T, A, V, F)               \
		template<T> struct ToBinary<A>                                             \
		{                                                                          \
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, V)          \
			{                                                                      \
				F                                                                  \
			}                                                                      \
		};  
		// リストをバイナリにシリアライズする特殊化クラスを作成するマクロです
	    #define _ELEKICORE_SERIALIZATION_LIST_TO_BINARY(T, A, V, B, E)             \
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(T, A, V,                          \
			binary->add((u8) EBinarySign::STRUCT);                                 \
			ToBinary<String>{}(binary, info, TXT("allocator"));                    \
			ToBinary<IAllocator *>{}(binary, info, value.allocator());             \
			ToBinary<String>{}(binary, info, TXT("elements"));                     \
			arrayToBinary(binary, info,  B, E);                                    \
			binary->add((u8) EBinarySign::END);                                    \
		);
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T COMMA size_t N, T[N], const T(&value)[N], arrayToBinary(binary COMMA info COMMA &value[0] COMMA &value[N]););
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T COMMA size_t N, Array<T COMMA N>, const Array<T COMMA N> &value, arrayToBinary(binary COMMA info COMMA  value.begin() COMMA value.end()););
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T, List<T>, const List<T> &value, value.begin(), value.end());
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class T, Set<T>, const Set<T> &value, value.begin(), value.end());
		_ELEKICORE_SERIALIZATION_ARRAY_TO_BINARY(class K COMMA class V, Map<K COMMA V>, const Map<K COMMA V> &value, value.begin(), value.end());

		// KeyValuePairをバイナリにシリアライズします
		template<class K, class V> struct ToBinary<KeyValuePair<K, V>>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const KeyValuePair<K, V> &value)
			{
				binary->add((u8) EBinarySign::STRUCT);
				ToBinary<String>{}(binary, info, TXT("key"));
				ToBinary<K>{}(binary, info, value.key);
				ToBinary<String>{}(binary, info, TXT("value"));
				ToBinary<V>{}(binary, info, value.value));
				binary->add((u8) EBinarySign::END);
			}
		};

		// 文字列をバイナリにシリアライズします
		template<> struct ToBinary<String>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const String &value)
			{
				binary->add((u8) EBinarySign::STRING);
				for(size_t i = 0; i < value.count(); i++)
				{
					numberToBinary(binary, value[i]);
				}
				binary->add((u8) EBinarySign::END);
			}
		};

		// データバイナリをシリアライズバイナリにシリアライズします
		template<> struct ToBinary<Binary>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const Binary &value)
			{
				size_t size = value.count();
				size_t index = 0;
				do
				{
					binary->add((u8) EBinarySign::BINARY);
					size_t length = size > U32_MAX ? U32_MAX : size;
					size -= length;

					ToBinary<u32>{}(binary, info, length);
					while(index != 0)
					{
						binary->add(value[index]);
						index++;
					}
				}
				while(size != 0);
				binary->add((u8) EBinarySign::END);
			}
		};

		/// シリアライズクラスを使いバイナリにシリアライズします
		template<class T, bool IS_ISERIALIZABLE = std::is_base_of_v<ISerializable, T>>
		struct SerializableToBinary
		{
			void operator()(Serializer &serializer, const T &value);
		};

		/// シリアライズクラスを使いバイナリにシリアライズします
		template<class T>
		struct SerializableToBinary<T, true>
		{
			void operator()(Serializer &serializer, const T &value);
		};
		
		// アロケータ情報をバイナリに組み込みますします
		template<> struct ToBinary<IAllocator *>
		{
			void operator()(Ref<List<u8>> binary, SerializeInfo &info, const IAllocator *&value)
			{
				auto serializable = dynamic_cast<const ISerializableAllocator *>(value);
				ToBinary<ISerializableAllocator>{}(binary, info, *serializable);
			}
		};



		//
		// デシリアライズ
		// ----- 

		/// データノード構造体です
		struct ELEKICORE_EXPORT DataNode
		{
			const EBinarySign type; ///< ノードのタイプです

			/// コンストラクタです
			DataNode(EBinarySign type);

			/// デストラクタです
			virtual ~DataNode();
		};

		// 数値のデシリアライズ用ノード構造体を作成するマクロです
		#define _ELEKICORE_SERIALIZATION_NUMBER_DATANODE(T, N) \
		struct ELEKICORE_EXPORT N##DataNode: DataNode          \
		{                                                      \
			T value;                                           \
			N##DataNode();                                     \
		};
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(i8, I8);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(u8, U8);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(i16, I16);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(u16, U16);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(i32, I32);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(u32, U32);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(i64, I64);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(u64, U64);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(f32, F32);
		_ELEKICORE_SERIALIZATION_NUMBER_DATANODE(f64, F64);

		// 定数のデシリアライズ用ノード構造体を作成するマクロです
		#define _ELEKICORE_SERIALIZATION_KEYWORD_DATANODE(N) \
		struct ELEKICORE_EXPORT N##DataNode: DataNode        \
		{                                                    \
			N##DataNode();                                   \
		};
		_ELEKICORE_SERIALIZATION_KEYWORD_DATANODE(True);
		_ELEKICORE_SERIALIZATION_KEYWORD_DATANODE(False);
		_ELEKICORE_SERIALIZATION_KEYWORD_DATANODE(Nil);

		/// 参照データノードタイプの列挙です
		enum class EReferenceNodeType
		{
			INSIDE,  ///< データ内完結参照
			OUTSIDE  ///< データ外部参照
		};

		/// 参照データノード構造体です
		struct ELEKICORE_EXPORT ReferenceDataNode: DataNode
		{
			const EReferenceNodeType referenceType; ///< 参照タイプ

			/// コンストラクタです
			ReferenceDataNode(EReferenceNodeType referenceType);

			/// デストラクタです
			virtual ~ReferenceDataNode() override;
		};

		/// 内部参照データノード構造体です
		struct ELEKICORE_EXPORT InsideReferenceDataNode: ReferenceDataNode
		{
			u32 value;

			/// コンストラクタです
			InsideReferenceDataNode();
		};

		/// 外部参照データノード構造体です
		struct ELEKICORE_EXPORT OutsideReferenceDataNode: ReferenceDataNode
		{
			String value;

			/// コンストラクタです
			OutsideReferenceDataNode();
		};

		/// 配列データノード構造体です
		struct ELEKICORE_EXPORT ArrayDataNode: DataNode
		{
			List<UR<DataNode>> value; ///< ノードの配列です

			/// コンストラクタです
			ArrayDataNode();
		};

		/// 構造データノード構造体です
		struct ELEKICORE_EXPORT StructDataNode: DataNode
		{
			Map<String, UR<DataNode>> value; ///< ノードのマップです

			/// コンストラクタです
			StructDataNode();
		};

		/// 文字列データノード構造体です
		struct ELEKICORE_EXPORT StringDataNode: DataNode
		{
			String value; ///< ノードの配列です

			/// コンストラクタです
			StringDataNode();
		};

		/// バイナリデータノード構造体です
		struct ELEKICORE_EXPORT BinaryDataNode: DataNode
		{
			Binary value; ///< ノードの配列です

			/// コンストラクタです
			BinaryDataNode();
		};

		/// バイナリデータからノードに変換します
		List<UR<DataNode>> ELEKICORE_EXPORT toDataNode(const UR<Binary> &binary);

		/// シリアライズ情報構造体です
		struct ELEKICORE_EXPORT DeserializeInfo
		{

			Map<String, void *> namedPtrMap; ///< 名前付き外部ポインタのマップ
			List<void *> allocatedPtrs;      ///< 確保済みポインタのリスト           !! -- toDataNode内で読み込みこんだサイズをここでも使用したい --

		};
		
		/// バイナリデータノードからデシリアライズします
		template<class T>
		struct FromBinary
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, T &value);
		};
		
		// バイナリデータノードを数値にデシリアライズする特殊化クラスを作成するマクロです
		#define _ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(T, SIGN, NAME)       \
		template<> struct FromBinary<T>                                          \
		{                                                                        \
			void operator()(Ref<DataNode> node, DeserializeInfo &info, T &value) \
			{                                                                    \
				if(node->type == SIGN)                                           \
				{                                                                \
					auto tNode = (Ref<NAME##DataNode>) node;                     \
					value = tNode->value;                                        \
				}                                                                \
			}                                                                    \
		};
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(i8, EBinarySign::I8, I8);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(u8, EBinarySign::U8, U8);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(i16, EBinarySign::I16, I16);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(u16, EBinarySign::U16, U16);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(i32, EBinarySign::I32, I32);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(u32, EBinarySign::U32, U32);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(i64, EBinarySign::I64, I64);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(u64, EBinarySign::U64, U64);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(f32, EBinarySign::F32, F32);
		_ELEKICORE_SERIALIZATION_NUMBER_FROM_BINARY(f64, EBinarySign::F64, F64);
		
		/// バイナリデータノードから論理値にデシリアライズします
		template<> struct FromBinary<bool>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, bool &value)
			{
				if (node->type == EBinarySign::TRUE) value = true;
				if (node->type == EBinarySign::FALSE) value = false;
			}
		};
		
		/// バイナリデータノードから参照にデシリアライズします
		template<class T> struct FromBinary<T&>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, T &value)
			{
				if (node->type == EBinarySign::REFERENCE)
				{
					auto refNode = (Ref<ReferenceDataNode>) node;
					if (refNode->referenceType == EReferenceNodeType::INPUT)
					{
						auto inRefNode = (Ref<InsideReferenceDataNode>) refNode;
						value = *(T*)info.allocatedPtrs[inRefNode];
					}
					else
					{
						auto outRefNode = (Ref<OutsideReferenceDataNode>) refNode;
						if (info.namedPtrMap.contains(outRefNode->value))
						{
							value = *(T*)info.namedPtrMap[outRefNode->value];
						}
					}
				}
			}
		};
		
		/// バイナリデータノードからポインタにデシリアライズします
		template<class T> struct FromBinary<T*>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, T &value)
			{
				if (node->type == EBinarySign::REFERENCE)
				{
					FromBinary<T&>{}(node, info, *value);
				}
				else
				{
					value = nullptr;
				}
			}
		};
		
		#define _ELEKICORE_SERIALIZATION_ARRAY_FROM_BINARY(T, A, V, F)    \
		template<T> struct FromBinary<A>                                  \
		{                                                                 \
			void operator()(Ref<DataNode> node, DeserializeInfo &info, V) \
			{                                                             \
				if (node->type == EBinarySign::ARRAY)                     \
				{                                                         \
					auto arrayNode = (Ref<ArrayDataNode>) node;           \
					auto &list = arrayNode->value;                        \
					F                                                     \
				}                                                         \
			}                                                             \
		};
		_ELEKICORE_SERIALIZATION_ARRAY_FROM_BINARY(class T COMMA size_t N, T[N], T (&value)[N], 
												   for(size_t i = 0; i < (N < list.count() ? N : list.count()); i++)
												   {
													   FromBinary<T>{}(list[i], info, value[i]);
												   }
												  );
		_ELEKICORE_SERIALIZATION_ARRAY_FROM_BINARY(class T COMMA size_t N, Array<T COMMA N>, Array<T COMMA N> &value, 
												   for(size_t i = 0; i < (N < list.count() ? N : list.count()); i++)
												   {
													   FromBinary<T>{}(list[i], info, value[i]);
												   }
												  );
		#define _ELEKICORE_SERIALIZATION_LIST_FROM_BINARY(T, A, V)                              \
		_ELEKICORE_SERIALIZATION_ARRAY_FROM_BINARY(T, A, V,                                     \
												   value.clear();                               \
												   for(auto &elemNode: list)                    \
												   {                                            \
													   char mem[sizeof(T)];                     \
													   T &tmpRef = *(T*)(void*)mem;             \
													   FromBinary<T>{}(elemNode, info, tmpRef); \
													   value.add(tmpRef);                       \
												   }                                            \
												  );
		_ELEKICORE_SERIALIZATION_LIST_FROM_BINARY(class T, List<T>, List<T> &value);
		_ELEKICORE_SERIALIZATION_LIST_FROM_BINARY(class T, Set<T>, Set<T> &value);
		_ELEKICORE_SERIALIZATION_LIST_FROM_BINARY(class K COMMA class V, Map<K COMMA V>, Map<K COMMA V> &value);
		
		/// バイナリデータノードからペア構造体にデシリアライズします
		template<class K, class V> struct FromBinary<KeyValuePair<K, V>>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, KeyValuePair<K, V> &value)
			{
				if (node->type == EBinarySign::STRUCT)
				{
					static const String keyName = TXT("key");
					static const String valueName = TXT("value");
					auto structNode = (Ref<StructDataNode>) node;
					if (node->value.contains(keyName)) FromBinary<K>{}(node->value[keyName]);
					if (node->value.contains(valueName)) FromBinary<K>{}(node->value[valueName]);
				}
			}
		};
		
		/// バイナリデータノードから文字列にデシリアライズします
		template<> struct FromBinary<String>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, String &value)
			{
				if (node->type == EBinarySign::STRING)
				{
					auto strNode = (Ref<StringDataNode>) node;
					value = strNode->value;
				}
			}
		};
		
		/// シリアライズバイナリデータノードからデータバイナリにデシリアライズします
		template<> struct FromBinary<Binary>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, Binary &value)
			{
				if (node->type == EBinarySign::BINARY)
				{
					auto binaryNode = (Ref<BinaryDataNode>) node;
					value = binaryNode->value;
				}
			}
		};
		
		/// シリアライズバイナリデータノードからデータバイナリにデシリアライズします
		template<> struct FromBinary<IAllocator *>
		{
			void operator()(Ref<DataNode> node, DeserializeInfo &info, IAllocator *&value)
			{
				if (node->type != EBinarySign::REFERENCE)
				{
					value = Memory::allocator();
					return;
				}
				
				auto refNode = (Ref<ReferenceDataNode>) node;
				if (refNode->referenceType == EReferenceNodeType::OUTSIDE)
				{
					auto outRefNode = (Ref<OutsideReferenceDataNode>) refNode;
					if (!info.namedPtrMap.contains(outRefNode->value))
					{
						value = Memory::allocator();
						return;
					}
				}
				
				auto serializable = dynamic_cast<ISerializableAllocator *>(value);
				if (!serializable)
				{
					value = Memory::allocator();
					return;
				}
				
				FromBinary<ISerializableAllocator&>(node, info, *serializable);
			}
		};
	}

	//
	// 実装
	// -----

	//
	// シリアライズ
	// -----

	/// シリアライザクラスです
	class ELEKICORE_EXPORT Serializer
	{
		Ref<List<u8>> mBinary;
		Serialization::SerializeInfo *mInfo;

	public:

		/// コンストラクタです
		Serializer(Ref<List<u8>> binary, Serialization::SerializeInfo *info);

		/// シリアライズします
		template<class T>
		Serializer &operator<<(const T &value)
		{
			Serialization::ToBinary<T>{}(mBinary, *mInfo, value);
			return *this;
		}

		/// シリアライズします
		template<class T>
		Serializer serialize(const T &value)
		{
			return ((*this) << value);
		}
	};

	/// デシリアライザクラスです
	class ELEKICORE_EXPORT Deserializer
	{

	};

	template<class T>
	inline void Serializable<T>::serialize(Serializer &serializer, const T &value)
	{
		// 特殊化してください
	}

	template<class T>
	inline void Serializable<T>::deserialize(Deserializer & deserializer, T & value)
	{
		// 特殊化してください
	}

	template<class T, bool IS_ISERIALIZABLE>
	inline void Serialization::SerializableToBinary<T, IS_ISERIALIZABLE>::operator()(Serializer &serializer, const T &value)
	{
		// ISerializable継承クラスをシリアライズします
		auto serializable = dynamic_cast<const ISerializable &>(value);
		serializable.serialize(serializer);
	}

	template<class T>
	inline void Serialization::SerializableToBinary<T, true>::operator()(Serializer &serializer, const T &value)
	{
		// 非侵入型Serializable特殊化構造体でシリアライズします
		Serializable<T>{}.serialize(serializer, value);
	}

	template<class T>
	inline void Serialization::ToBinary<T>::operator()(Ref<List<u8>> binary, Serialization::SerializeInfo &info, const T &value)
	{
		// ISerializable継承クラス、
		// または、非侵入型Serializable特殊化構造体の
		// シリアライズ関数を呼び出します
		binary->add((u8) Serialization::EBinarySign::STRUCT);
		Serializer serializer(binary, &info);
		Serialization::SerializableToBinary<T>{}(serializer, value);
		binary->add((u8) Serialization::EBinarySign::END);
	}

	// 
	// シリアライズ関数
	// -----

	/// バイナリにシリアライズします
	/// @param value シリアライズ対象
	/// @param names 非シリアライズ対象のポインタと名前
	/// @return バイナリデータ
	template<class T>
	UR<List<u8>> toBinary(const T &value, const Map<void*, String> &names = Map<void *, String>())
	{
		// インスタンスデータサイズの型
		using InstanceSizeType = u32;
		// インスタンスデータサイズ部のサイズ
		static constexpr size_t INSTANCE_DATA_SIZE_DATA_SIZE = sizeof(InstanceSizeType);

		// インスタンスを並列にシリアライズします
		Serialization::SerializeInfo info;
		{
			// バイナリリストとinfoを作成します
			Ref<List<u8>> refBinary;
			{
				auto urBinary = newUR<List<u8>>();
				refBinary = urBinary;
				info.binaryList.add(urBinary);
				info.ptrNameMap = names;
			}

			// インスタンス単位で並列にシリアライズします
			{
				Serialization::ToBinary<T>{}(refBinary, info, value);
				for(auto &task : info.taskList) task->marge();
			}
		}

		// 1本化します
		UR<List<u8>> resultBinary;
		{
			// 1本化をインスタンスデータ単位で行うために、
			// ヘッダー部の分を加えた状態で
			// 各インスタンスデータの挿入位置とサイズを配列に挿入します
			//
			// デシリアライズをインスタンス単位で行うために、
			// インスタンスデータ長を記録します
			auto size = Serialization::BinaryInformation::SIZE;
			List<size_t> startPos;
			List<size_t> instanceSize;
			for(auto &binary : info.binaryList)
			{
				startPos.add(size);
				size += INSTANCE_DATA_SIZE_DATA_SIZE + binary->count();
				instanceSize.add(binary->count());
			}

			// 1本化用バイナリリスト
			resultBinary = newUR<List<u8>>(size);
			Ref<List<u8>> refBinary = urBinary;
			List<UR<Task<void>>> tasks(size);

			// インスタンスデータ毎に並列に1本化します
			for(size_t i = 0; i < info.binaryList.count(); i++)
			{
				tasks[i] = parallel([&info, &startPos, &instanceSize, refBinary, i]()
				{

					Serialization::numberToBinary<InstanceSizeType>(refBinary, instanceSize[i]);
					for(size_t j = INSTANCE_DATA_SIZE_DATA_SIZE + startPos[i], k = 0; k < instanceSize[i]; j++, k++)
					{
						refBinary->at(j) = info.binaryList[i]->at(k);
					}
				});
			}
		}
		
		// ヘッダ情報を含ませます
		{
			// ELEKIBINARY ASC2文字列を含ませます
			for(size_t i = 0; i < Serialization::BinaryInformation::NAME.count(); i++)
			{
				resultBinary[i] = Serialization::BinaryInformation::NAME[i];
			}
			// バージョンを含ませます
			for(size_t i = Serialization::BinaryInformation::NAME.count(), j = 0; j < sizeof(Serialization::BinaryInformation::VERSION); i++, j++)
			{
				resultBinary[i] = ((u8 *) (void *) &Serialization::BinaryInformation::VERSION)[j];
			}
		}

		// 並列処理の終了を待ちます
		for(auto &task : tasks) task->marge();

		return resultBinary;
	}

	/// バイナリからデシリアライズします
	/// @param value デシリアライズ対象
	/// @param binary バイナリデータ
	/// @param names 構築済み外部ポインタと名前
	/// @return デシリアライズ成功の真偽
	template<class T>
	bool fromBinary(T &value, const UR<List<u8>> &binary, const Map<String, void *> &names = Map<String, void *>())
	{

	}
}

#endif // !ELEKICORE_SERIALIZATION_HPP
