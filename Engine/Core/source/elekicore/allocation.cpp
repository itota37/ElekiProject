#include <new>
#include <mutex>
#include "elekicore/allocation.hpp"

#include <iostream>

using namespace ElekiEngine;

#pragma warning(push)
#pragma warning(disable:6001)
#pragma warning(disable:6011)
#pragma warning(disable:6386)

//
// StaticFrameMemory
// -----

// コンストラクタ
// @param bufferSize 確保するバッファサイス
ElekiEngine::StaticFrameMemory::StaticFrameMemory(size_t bufferSize)
	: mBufferSize((!bufferSize ? sizeof(u8) : bufferSize))
	, mUseSize(0)
	, mBuffer(new(std::malloc(mBufferSize)) u8())
{}

// デストラクタ
ElekiEngine::StaticFrameMemory::~StaticFrameMemory()
{
	std::free(mBuffer);
}

// メモリを確保します
// @param byteSize 確保するメモリサイズ
// @retval nullptr メモリの確保に失敗しました
void *ElekiEngine::StaticFrameMemory::allocate(size_t byteSize)
{
	// 使用可能なサイズが要求サイズ未満の場合nullptr
	if((mBufferSize - mUseSize) < byteSize) return nullptr;

	// 未使用域の先頭から分割し、使用サイズ分進める
	auto tmp = &mBuffer[mUseSize];
	mUseSize += byteSize;
	return tmp;
}

// メモリを一括で解放します
void ElekiEngine::StaticFrameMemory::deallocate()
{
	mUseSize = 0;
}

// バッファサイズを返します
size_t ElekiEngine::StaticFrameMemory::bufferSize() const
{
	return mBufferSize;
}

// 使用済みサイズを返します
size_t ElekiEngine::StaticFrameMemory::useSize() const
{
	return mUseSize;
}

// =====
// DynamicFrameMemory
// -----

// コンストラクタ
ElekiEngine::DynamicFrameMemory::Node::Node(size_t bufferSize, Node *prev)
	: mMemory(bufferSize)
	, mPrev(prev)
{}

// デストラクタ
ElekiEngine::DynamicFrameMemory::Node::~Node()
{}

// コンストラクタ
// @param bufferSize 確保するバッファサイス
ElekiEngine::DynamicFrameMemory::DynamicFrameMemory(size_t bufferSize)
	: mBufferSize(bufferSize)
	, mTopNode(new(std::malloc(sizeof(Node))) Node(mBufferSize, nullptr))
{}

// デストラクタ
ElekiEngine::DynamicFrameMemory::~DynamicFrameMemory()
{
	// すべてのノードを解放
	while(mTopNode)
	{
		auto tmp = mTopNode;
		mTopNode = mTopNode->mPrev;
		tmp->~Node();
		std::free(tmp);
	}
}

// メモリを確保します
// @param byteSize 確保するメモリサイズ
// @retval nullptr メモリの確保に失敗しました
void *ElekiEngine::DynamicFrameMemory::allocate(size_t byteSize)
{
	// バッファサイズが要求サイズ未満の場合nullptr
	if(mBufferSize < byteSize) return nullptr;

	// 使用可能なサイズが要求サイズ未満の場合ノードを追加
	if((mBufferSize - mTopNode->mMemory.useSize()) < byteSize)
	{
		mTopNode = new(std::malloc(sizeof(Node))) Node(mBufferSize, mTopNode);
	}

	// メモリ確保
	return mTopNode->mMemory.allocate(byteSize);
}

// メモリを一括で解放します
void ElekiEngine::DynamicFrameMemory::deallocate()
{
	// 初期ノードを残して解放
	while(mTopNode->mPrev)
	{
		auto tmp = mTopNode;
		mTopNode = mTopNode->mPrev;
		tmp->~Node();
		std::free(tmp);
	}

	// 初期ノードのバッファメモリ解放
	mTopNode->mMemory.deallocate();
}

// バッファサイズを返します
size_t ElekiEngine::DynamicFrameMemory::bufferSize() const
{
	return mBufferSize;
}

//
// StaticMemoryPool
// -----

// コンストラクタ
ElekiEngine::StaticMemoryPool::StaticMemoryPool(size_t elementSize, size_t elementsCount)
	: mElementSize((elementSize > sizeof(size_t) ? elementSize : sizeof(size_t)))
	, mElementsCount((!elementsCount ? sizeof(u8) : elementsCount))
	, mFreeElementsCount((!elementsCount ? sizeof(u8) : elementsCount))
	, mBuffer(new(std::malloc(mElementSize * mElementsCount)) u8())
	, mFreeElementsLinkTop(nullptr)
{
	// mBuffer[i]の値にmBuffer[i-mElementSize]のアドレスを代入
	for(int i = mElementSize * (mElementsCount - 1); i >= 0; i-=mElementSize)
	{
		reinterpret_cast<u8*&>(mBuffer[i]) = mFreeElementsLinkTop;
		mFreeElementsLinkTop = &mBuffer[i];
	}
}

// デストラクタ
ElekiEngine::StaticMemoryPool::~StaticMemoryPool()
{
	std::free(mBuffer);
}

// メモリを確保します
// @retval nullptr メモリの確保に失敗しました
void *ElekiEngine::StaticMemoryPool::allocate()
{
	if(!mFreeElementsLinkTop) return nullptr;

	auto tmp = mFreeElementsLinkTop;
	// フリーリンクの先頭の値をアドレスに変換し、次の先頭とする
	mFreeElementsLinkTop = reinterpret_cast<u8*&>(*mFreeElementsLinkTop);
	mFreeElementsCount--;
	return tmp;
}

// メモリを解放します
// @param pointer 解放するポインタ
void ElekiEngine::StaticMemoryPool::deallocate(void *pointer)
{
	if(!pointer) return;

	mFreeElementsCount++;
	// フリーリンクの先頭が指す先の値を戻したポインタの値に代入
	(*(u8 **)pointer) = mFreeElementsLinkTop;
	mFreeElementsLinkTop = (u8 *) pointer;
}

// 要素サイズを返します
size_t ElekiEngine::StaticMemoryPool::elementSize() const
{
	return mElementSize;
}

// 要素数を返します
size_t ElekiEngine::StaticMemoryPool::elementsCount() const
{
	return mElementsCount;
}

// 未使用要素数を返します
size_t ElekiEngine::StaticMemoryPool::freeElementsCount() const
{
	return mFreeElementsCount;
}

//
// DynamicMemoryPool
// -----

// コンストラクタ
ElekiEngine::DynamicMemoryPool::Node::Node(size_t elementSize, size_t elementsCount, DynamicMemoryPool *system, Node *prev, Node *next)
	: mMemory(elementSize, elementsCount)
	, mSystem(system)
	, mPrev(prev)
	, mNext(next)
{}

// デストラクタ
ElekiEngine::DynamicMemoryPool::Node::~Node()
{}

// コンストラクタ
// @param elementSize 要素サイズ
// @param elementsCount 要素数
ElekiEngine::DynamicMemoryPool::DynamicMemoryPool(size_t elementSize, size_t elementsCount)
	: mElementSize(elementSize)
	, mElementsCount(elementsCount)
	, mTopNode(new(std::malloc(sizeof(Node))) Node(sizeof(size_t) + mElementSize, mElementsCount, this, nullptr, nullptr))
{
	mTopNode->mPrev = mTopNode;
	mTopNode->mNext = mTopNode;
}

// デストラクタ
ElekiEngine::DynamicMemoryPool::~DynamicMemoryPool()
{
	// すべてのノードを解放
	while(mTopNode)
	{
		auto tmp = mTopNode;
		mTopNode = mTopNode->mPrev;
		tmp->~Node();
		std::free(tmp);
	}
}

// メモリを確保します
// @retval nullptr メモリの確保に失敗しました
void *ElekiEngine::DynamicMemoryPool::allocate()
{
	// 使用可能な要素がない場合ノードを追加
	if(!mTopNode->mMemory.freeElementsCount())
	{
		// [prev]-[old-top]
		//   ↓
		// [prev]-[new-top]-[old-top]
		mTopNode = new(std::malloc(sizeof(Node))) Node(sizeof(size_t) + mElementSize, mElementsCount, this, mTopNode->mPrev, mTopNode);
		mTopNode->mPrev->mNext = mTopNode;
		mTopNode->mNext->mPrev = mTopNode;
	}

	// アドレスサイズ＋要素サイズのメモリを確保
	auto tmp = (Node **) mTopNode->mMemory.allocate();
	tmp[0] = mTopNode;
	return &tmp[1];
}

// メモリを解放します
// @param pointer 解放するポインタ
void ElekiEngine::DynamicMemoryPool::deallocate(void *pointer)
{
	auto tmp = &((Node **) pointer)[-1];
	auto node = *tmp;
	node->mMemory.deallocate(tmp);

	if(node != node->mSystem->mTopNode && node->mMemory.freeElementsCount() == node->mMemory.elementsCount())
	{
		node->mPrev->mNext = node->mNext;
		node->mNext->mPrev = node->mPrev;
		node->~Node();
		std::free(node);
	}
}

// 要素サイズを返します
size_t ElekiEngine::DynamicMemoryPool::elementSize() const
{
	return mElementSize;
}

//
// Memory
// -----

// 256以上のメモリを確保するメモリシステム
class MallocMemory
{
public:

	// メモリを確保します
	// @param byteSize 確保するメモリサイズ
	void *allocate(size_t byteSize)
	{
		auto tmp = (u8 **) std::malloc(sizeof(u8 *) + byteSize);
		tmp[0] = nullptr;
		return &tmp[1];
	}

	// メモリを解放します
	// @param pointer 解放するポインタ
	void deallocate(void *pointer)
	{
		auto tmp = &((u8 **) pointer)[-1];
		std::free(tmp);
	}
};

// メモリを管理する構造体
struct MemoryControl
{
	DynamicMemoryPool size16Pool;
	DynamicMemoryPool size32Pool;
	DynamicMemoryPool size64Pool;
	DynamicMemoryPool size128Pool;
	DynamicMemoryPool size256Pool;
	MallocMemory sizeoverMemory;

	static constexpr size_t SIZE16 = 16;
	static constexpr size_t SIZE32 = 16;
	static constexpr size_t SIZE64 = 16;
	static constexpr size_t SIZE128 = 16;
	static constexpr size_t SIZE256 = 16;

	static constexpr size_t SIZE16_CNT = 32;
	static constexpr size_t SIZE32_CNT = 32;
	static constexpr size_t SIZE64_CNT = 32;
	static constexpr size_t SIZE128_CNT = 16;
	static constexpr size_t SIZE256_CNT = 16;

	// コンストラクタ
	MemoryControl()
		: size16Pool(SIZE16, SIZE16_CNT)
		, size32Pool(SIZE32, SIZE32_CNT)
		, size64Pool(SIZE64, SIZE64_CNT)
		, size128Pool(SIZE128, SIZE128_CNT)
		, size256Pool(SIZE256, SIZE256_CNT)
		, sizeoverMemory()
	{}

	// メモリを確保します
	// @param byteSize 確保するメモリサイズ
	// @retval nullptr メモリの確保に失敗しました
	void *allocate(size_t byteSize)
	{
		if(!byteSize) return nullptr;
		
		if(byteSize <= SIZE16) return size16Pool.allocate();
		if(byteSize <= SIZE32) return size32Pool.allocate();
		if(byteSize <= SIZE64) return size64Pool.allocate();
		if(byteSize <= SIZE128) return size128Pool.allocate();
		if(byteSize <= SIZE256) return size256Pool.allocate();
		return sizeoverMemory.allocate(byteSize);
	}

	// メモリを解放します
	// @param pointer 解放するポインタ
	void deallocate(void *pointer)
	{
		if(((u8 **) pointer)[-1])
		{
			DynamicMemoryPool::deallocate(pointer);
		}
		else
		{
			sizeoverMemory.deallocate(pointer);
		}
	}
};

MemoryControl *gMemoryControl;      // 共有メモリ
std::once_flag gInitMemoryControlF; // initMemoryControlの呼び出しフラグ
// gMemoryControlを初期化します
void initMemoryControl()
{
	gMemoryControl = new(std::malloc(sizeof(MemoryControl))) MemoryControl();
}

// メモリを確保します
// @param byteSize 確保するメモリサイズ
// @retval nullptr メモリの確保に失敗しました
void *ElekiEngine::Memory::allocate(size_t byteSize)
{
	std::call_once(gInitMemoryControlF, initMemoryControl);
	return gMemoryControl->allocate(byteSize);
}

// メモリを解放します
// @param pointer 解放するポインタ
void ElekiEngine::Memory::deallocate(void *pointer)
{
	std::call_once(gInitMemoryControlF, initMemoryControl);
	gMemoryControl->deallocate(pointer);
}

// 共有アロケータです
class GlobalAllocator: public IAllocator
{
public:

	// メモリを確保します
	// @param byteSize 確保するメモリサイズ
	// @retval nullptr メモリの確保に失敗しました
	void *allocate(size_t byteSize) override
	{
		return Memory::allocate(byteSize);
	}

	// メモリを解放します
	// @param pointer 解放するポインタ
	void deallocate(void *pointer) override
	{
		Memory::deallocate(pointer);
	}
};

IAllocator *gAllocator; // 共有アロケータ
std::once_flag gInitAllocatorF;
void initAllocator()
{
	gAllocator = new(std::malloc(sizeof(GlobalAllocator))) GlobalAllocator();
}

// 共有アロケータです
// @return アロケータのポインタ
IAllocator *ElekiEngine::Memory::allocator()
{
	std::call_once(gInitAllocatorF, initAllocator);
	return gAllocator;
}

// 共有デリータ
class GlobalDeleter: public IDeleter
{
public:

	// 終了処理を実行します
	void operator()(void *pointer) override
	{
		Memory::deallocate(pointer);
	}
};

IDeleter *gDeleter; // 共有デリータ
std::once_flag gInitDeleterF;
void initDeleter()
{
	gDeleter = new(std::malloc(sizeof(GlobalDeleter))) GlobalDeleter();
}

// 共有デリータです
// @return デリータのポインタ
IDeleter *ElekiEngine::Memory::deleter()
{
	std::call_once(gInitDeleterF, initDeleter);
	return gDeleter;
}

#pragma warning(pop)