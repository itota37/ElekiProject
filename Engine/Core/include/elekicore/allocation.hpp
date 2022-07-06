/// @file allocation.hpp
/// @version 1.22.4
/// @copyright © 2022 Taichi Ito
/// メモリ確保、解放処理機能を提供します

#ifndef ELEKICORE_ALLOCATION_HPP
#define ELEKICORE_ALLOCATION_HPP

#include "preprocess.hpp"
#include "integer.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{
    
    /// 一括確保したメモリを先頭から順に使用し、一括で解放するメモリ管理システムです
    class ELEKICORE_EXPORT StaticFrameMemory
    {
        size_t mBufferSize; // バッファサイズ
        size_t mUseSize;    // 使用済みサイズ
        u8 *mBuffer;        // バッファポインタ

    public:

        /// コンストラクタ
        /// @param bufferSize 確保するバッファサイス
        StaticFrameMemory(size_t bufferSize);

        /// デストラクタ
        ~StaticFrameMemory();

        /// メモリを確保します
        /// @param byteSize 確保するメモリサイズ
        /// @retval nullptr メモリの確保に失敗しました
        void *allocate(size_t byteSize);

        /// メモリを一括で解放します
        void deallocate();

        /// バッファサイズを返します
        size_t bufferSize() const;

        /// 使用済みサイズを返します
        size_t useSize() const;
    };

    /// メモリを先頭から順に使用し、一括で解放するメモリ管理システムです
    class ELEKICORE_EXPORT DynamicFrameMemory
    {
        // バッファ管理ノード
        struct Node
        {
            StaticFrameMemory mMemory; // バッファ管理
            Node *mPrev;               // 前に使用していたノード

            // コンストラクタ
            Node(size_t bufferSize, Node *prev);

            // デストラクタ
            ~Node();
        };

        size_t mBufferSize; // 1バッファのサイズ
        Node *mTopNode;     // 現在使用中のノード

    public:

        /// コンストラクタ
        /// @param bufferSize 確保するバッファサイス
        DynamicFrameMemory(size_t bufferSize);

        /// デストラクタ
        ~DynamicFrameMemory();

        /// メモリを確保します
        /// @param byteSize 確保するメモリサイズ
        /// @retval nullptr メモリの確保に失敗しました
        void *allocate(size_t byteSize);

        /// メモリを一括で解放します
        void deallocate();

        /// バッファサイズを返します
        size_t bufferSize() const;
    };

    /// 固定長バッファから固定長のメモリを確保、解放するメモリシステムです
    class ELEKICORE_EXPORT StaticMemoryPool
    {
        size_t mElementSize;       // 要素サイズ
        size_t mElementsCount;     // 要素数
        size_t mFreeElementsCount; // 未使用要素数
        u8 *mBuffer;               // バッファポインタ
        u8 *mFreeElementsLinkTop;  // 未使用要素連結リストの先頭

    public:

        /// コンストラクタ
        /// @param elementSize 要素サイズ
        /// @param elementsCount 要素数
        StaticMemoryPool(size_t elementSize, size_t elementsCount);

        /// デストラクタ
        ~StaticMemoryPool();

        /// メモリを確保します
        /// @retval nullptr メモリの確保に失敗しました
        void *allocate();

        /// メモリを解放します
        /// @param pointer 解放するポインタ
        void deallocate(void *pointer);

        /// 要素サイズを返します
        size_t elementSize() const;

        /// 要素数を返します
        size_t elementsCount() const;

        /// 未使用要素数を返します
        size_t freeElementsCount() const;
    };

    /// 固定長バッファから固定長のメモリを確保、解放するメモリシステムです
    class ELEKICORE_EXPORT DynamicMemoryPool
    {
        // バッファ管理ノード
        struct Node
        {
            StaticMemoryPool mMemory;   // バッファ管理
            DynamicMemoryPool *mSystem; // このノードを管理するシステム
            Node *mPrev;                // 前ノード
            Node *mNext;                // 次ノード

            // コンストラクタ
            Node(size_t elementSize, size_t elementsCount, DynamicMemoryPool *system, Node *prev, Node *next);

            // デストラクタ
            ~Node();
        };

        size_t mElementSize;   // 要素サイズ
        size_t mElementsCount; // 要素数
        Node *mTopNode;        // 現在使用中のノード

    public:

        /// コンストラクタ
        /// @param elementSize 要素サイズ
        /// @param elementsCount 要素数
        DynamicMemoryPool(size_t elementSize, size_t elementsCount);

        /// デストラクタ
        ~DynamicMemoryPool();

        /// メモリを確保します
        /// @retval nullptr メモリの確保に失敗しました
        void *allocate();

        /// メモリを解放します
        /// @param pointer 解放するポインタ
        static void deallocate(void *pointer);

        /// 要素サイズを返します
        size_t elementSize() const;
    };

    /// メモリシステムインタフェースです
    class IAllocator
    {
    public:

        /// メモリを確保します
        /// @param byteSize 確保するメモリサイズ
        /// @retval nullptr メモリの確保に失敗しました
        virtual void *allocate(size_t byteSize) = 0;

        /// メモリを解放します
        /// @param pointer 解放するポインタ
        virtual void deallocate(void *pointer) = 0;
    };

    /// 終了処理インタフェース
    class IDeleter
    {
    public:

        /// 終了処理を実行します
        virtual void operator()(void *pointer) = 0;
    };

    /// システム共有メモリを提供する静的クラスです
    class ELEKICORE_EXPORT Memory
    {
    public:

        /// メモリを確保します
        /// @param byteSize 確保するメモリサイズ
        /// @retval nullptr メモリの確保に失敗しました
        static void *allocate(size_t byteSize);

        /// メモリを解放します
        /// @param pointer 解放するポインタ
        static void deallocate(void *pointer);

        /// 共有アロケータです
        /// @return アロケータのポインタ
        static IAllocator *allocator();

        /// 共有デリータです
        /// @return デリータのポインタ
        static IDeleter *deleter();
    };
}

#endif // !ELEKICORE_ALLOCATION_HPP