#include "elekicore/serialization.hpp"

using namespace ElekiEngine;

// コンストラクタ
// @param count 初期要素数
// @param allocator 使用するアロケータ
ElekiEngine::Binary::Binary(size_t count, IAllocator *allocator = Memory::allocator())
	: List<u8>(count, allocator)
{}

// コンストラクタ
// @param allocator 使用するアロケータ
ElekiEngine::Binary::Binary(IAllocator *allocator = Memory::allocator())
	: List<u8>(allocator)
{}

// コンストラクタ
// @param list 初期化リスト
// @param allocator 使用するアロケータ
ElekiEngine::Binary::Binary(std::initializer_list<u8> list, IAllocator *allocator = Memory::allocator())
	: List<u8>(list, allocator)
{}

// コピーコンストラクタ
ElekiEngine::Binary::Binary(const Binary &binary)
	: List<u8>(binary)
{}

// ムーブコンストラクタ
ElekiEngine::Binary::Binary(Binary &&binary) noexcept
	: List<u8>(binary)
{}

// ムーブコンストラクタ
ElekiEngine::Binary::~Binary()
{}

// コピー代入します
Binary &ElekiEngine::Binary::operator=(const Binary & binary)
{
	List<u8>::operator=(binary);
	return *this;
}

// ムーブ代入します
Binary &ElekiEngine::Binary::operator=(Binary &&binary) noexcept
{
	List<u8>::operator=(binary);
	return *this;
}

// 末尾に追加します
Binary &ElekiEngine::Binary::operator+=(const Binary &binary)
{
	List<u8>::operator+=(binary);
	return *this;
}

// 末尾に追加します
Binary &ElekiEngine::Binary::operator+=(Binary &&binary) noexcept
{
	List<u8>::operator+=(binary);
	return *this;
}

// 末尾に追加します
Binary &ElekiEngine::Binary::operator+=(const u8 &element)
{
	List<u8>::operator+=(element);
	return *this;
}

// 末尾に追加します
Binary &ElekiEngine::Binary::operator+=(u8 &&element) noexcept
{
	List<u8>::operator+=(element);
	return *this;
}

// 添え字から要素にアクセスします
u8 &ElekiEngine::Binary::operator[](size_t index)
{
	return List<u8>::operator[](index);
}

// 添え字から要素にアクセスします
const u8 &ElekiEngine::Binary::operator[](size_t index) const
{
	return List<u8>::operator[](index);
}

// 連結します
Binary ElekiEngine::operator+(const Binary &l, const Binary &r)
{
	return Binary(l) += r;
}

const ElekiEngine::String ElekiEngine::Serialization::BinaryInformation::NAME = TXT("ELEKIBINARY");

//
// シリアライズ
// -----

// コンストラクタです
ElekiEngine::Serializer::Serializer(Ref<List<u8>> binary, Serialization::SerializeInfo *info)
	: mBinary(binary)
	, mInfo(info)
{}

//
// デシリアライズ
// -----

// コンストラクタです
ElekiEngine::Serialization::DataNode::DataNode(EBinarySign type)
	: type(type)
{}

// デストラクタです
ElekiEngine::Serialization::DataNode::~DataNode()
{}

// デシリアライズ用ノード構造体のコンストラクタを作成するマクロです
#define _ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(N, S) \
ElekiEngine::Serialization::N##DataNode::N##DataNode()      \
	: DataNode(EBinarySign::S)                              \
{}
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(I8, I8);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(U8, U8);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(I16, I16);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(U16, U16);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(I32, I32);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(U32, U32);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(I64, I64);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(U64, U64);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(F32, F32);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(F64, F64);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(True, TRUE);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(False, FALSE);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(Nil, NIL);
// コンストラクタです
ElekiEngine::Serialization::ReferenceDataNode::ReferenceDataNode(EReferenceNodeType referenceType)
	: DataNode(EBinarySign::REFERENCE)
	, referenceType(referenceType)
{}
// デストラクタです
ElekiEngine::Serialization::ReferenceDataNode::~ReferenceDataNode()
{}
// コンストラクタです
ElekiEngine::Serialization::InsideReferenceDataNode::InsideReferenceDataNode()
	: ReferenceDataNode(EReferenceNodeType::INSIDE)
{}
// コンストラクタです
ElekiEngine::Serialization::OutsideReferenceDataNode::OutsideReferenceDataNode()
	: ReferenceDataNode(EReferenceNodeType::OUTSIDE)
{}
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(Array, ARRAY);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(Struct, STRUCT);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(String, STRING);
_ELEKICORE_SERIALIZATION_DATANODE_CONSTRUCTER(Binary, BINARY);


//
// データからノードに変換
// -----

template<class T, size_t SIZE = sizeof(T)>
T readBinaryNumber(const Ref<List<u8>> &binary, size_t index)
{
	T result;
	auto outBytes = (u8 *) (void *) &result;
	auto inBytes = (const u8 *) (const void *) &binary->at(index);

	switch(endian())
	{
		case EEndian::LITTLE:
		for(size_t i = 0; i < SIZE; i++) outBytes[i] = inBytes[i];
		break;

		case EEndian::BIG:
		for(int i = SIZE - 1; i >= 0; i--) outBytes[i] = inBytes[i];
		break;
	}

	return result;
}

bool isElekiBinary(const UR<List<u8>> &binary)
{
	size_t i = 0;
	// 形式名を比較
	for(; i < Serialization::BinaryInformation::NAME.count(); i++)
	{
		if(Serialization::BinaryInformation::NAME[i] != (Char) binary->at(i)) return;
	}
	// バージョンを比較
	return Serialization::BinaryInformation::VERSION == readBinaryNumber<u32>(binary, i);
}

// バイナリデータからノードに変換します
List<UR<Serialization::DataNode>> ElekiEngine::Serialization::toDataNode(const UR<List<u8>> &binary)
{
	if(!isElekiBinary(binary)) return List<UR<Serialization::DataNode>>();
}
