/// @file pointer.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// ファイルの説明(複数行可)

#ifndef ELEKICORE_POINTER_HPP
#define ELEKICORE_POINTER_HPP

#include <new>
#include "allocation.hpp"
#include "datalog.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{
    /// 弱い参照クラスです
    template<class T>
    class Ref;

    /// ユニーク参照クラスです
    template<class T>
    class UR;

    /// 参照カウンタクラスです
    template<class T>
    class RC;

    /// 弱い参照クラスです
    template<class T>
    class Ref
    {
        template<class T> friend class UR;
        template<class T> friend class RC;
        friend class Ref<void>;

        struct Info
        {
            void *pointer;
            size_t count;
            IDeleter *deleter;
            Ref<u8> *top;
        };

        Info          *mInfo; // 参照情報
        Ref<u8>       *mPrev; // 前の参照
        Ref<u8>       *mNext; // 次の参照

        // 接続します
        static void connect(Info *&info, Ref<u8> *&prev, Ref<u8> *&next)
        {
            if(info)
            {
                if(info->top)
                {
                    prev = info->top->mPrev;
                    next = info->top;
                }
                else
                {
                    prev = (Ref<u8>*)this;
                    next = (Ref<u8>*)this;
                    info->top = (Ref<u8>*)this;
                }
                info->top->mPrev->mNext = (Ref<u8>*)this;
                info->top->mPrev = (Ref<u8>*)this;
            }
        }

        // 切断します
        static void disconnect(Info *&info, Ref<u8> *&prev, Ref<u8> *&next)
        {
            if(info)
            {
                if(info->top == (Ref<u8>*)this) info->top = next;
                prev->mNext = next;
                next->mPrev = prev;
            }
        }

        /// コンストラクタ
        Ref(Info *info)
            : mInfo(info)
            , mPrev(nullptr)
            , mNext(nullptr)
        {
            connect(mInfo, mPrev, mNext);
        }

    public:

        /// コンストラクタ
        Ref()
            : mInfo(nullptr)
            , mPrev(nullptr)
            , mNext(nullptr)
        {}

        /// コピーコンストラクタ
        Ref(const Ref<T> &ref)
            : mInfo(ref.mInfo)
            , mPrev(nullptr)
            , mNext(nullptr)
        {
            connect(mInfo, mPrev, mNext);
        }

        /// コピーコンストラクタ
        Ref(Ref<T> &&ref) noexcept
            : mInfo(ref.mInfo)
            , mPrev(nullptr)
            , mNext(nullptr)
        {
            connect(mInfo, mPrev, mNext);
        }

        /// デストラクタ
        ~Ref()
        {
            disconnect(mInfo, mPrev, mNext);
        }

        /// 代入演算子
        Ref<T> &operator=(const Ref<T> &ref)
        {
            disconnect(mInfo, mPrev, mNext);
            mInfo = ref.mInfo;
            connect(mInfo, mPrev, mNext);
            return *this;
        }

        /// 代入演算子
        Ref<T> &operator=(Ref<T> &&ref) noexcept
        {
            disconnect(mInfo, mPrev, mNext);
            mInfo = ref.mInfo;
            connect(mInfo, mPrev, mNext);
            return *this;
        }

        /// 参照先を取得します
        T &operator*()
        {
            if(!mInfo) printError("null pointer. T &Ref<T>::operator*()");
            return *(T *) mInfo->pointer;
        }

        /// 参照先を取得します
        const T &operator*() const
        {
            if(!mInfo) printError("null pointer. const T &Ref<T>::operator*() const");
            return *(T *) mInfo->pointer;
        }

        /// 参照先にアクセスします
        T *operator->()
        {
            if(!mInfo) printError("null pointer. T *Ref<T>::operator->()");
            return (T *) mInfo->pointer;
        }

        /// キャストします
        template<class U>
        explicit operator Ref<U>() const
        {
            return Ref<U>(mInfo);
        }

        /// 参照先を取得します
        T &reference()
        {
            if(!mInfo) printError("null pointer. T &Ref<T>::reference()");
            return *(T *) mInfo->pointer;
        }

        /// 参照先を取得します
        const T &reference() const
        {
            if(!mInfo) printError("null pointer. const T &Ref<T>::reference() const");
            return *(T *) mInfo->pointer;
        }

        /// 参照先にアクセスします
        T *pointer()
        {
            return (mInfo ? (T *) mInfo->top : nullptr);
        }

        /// 参照先にアクセスします
        const T *pointer() const
        {
            return (mInfo ? (T *) mInfo->top : nullptr);
        }

        /// 参照カウントを返します
        size_t count() const
        {
            return (mInfo ? mInfo->count : 0);
        }
    };

    /// 弱い参照クラスです
    template<>
    class Ref<void>
    {
        template<class T> friend class UR;
        template<class T> friend class RC;

        Ref<u8>::Info *mInfo;   // 参照情報
        Ref<u8> *mPrev;         // 前の参照
        Ref<u8> *mNext;         // 次の参照
        
        // コンストラクタ
        Ref(Ref<u8>::Info *info)
            : mInfo(info)
            , mPrev(nullptr)
            , mNext(nullptr)
        {
            Ref<u8>::connect(mInfo, mPrev, mNext);
        }

    public:

        /// コンストラクタ
        Ref()
            : mInfo(nullptr)
            , mPrev(nullptr)
            , mNext(nullptr)
        {}

        /// コピーコンストラクタ
        Ref(const Ref<void> &ref)
            : mInfo(ref.mInfo)
            , mPrev(nullptr)
            , mNext(nullptr)
        {
            Ref<u8>::connect(mInfo, mPrev, mNext);
        }

        /// コピーコンストラクタ
        Ref(Ref<void> &&ref) noexcept
            : mInfo(ref.mInfo)
            , mPrev(nullptr)
            , mNext(nullptr)
        {
            Ref<u8>::connect(mInfo, mPrev, mNext);
        }

        /// デストラクタ
        ~Ref()
        {
            Ref<u8>::disconnect(mInfo, mPrev, mNext);
        }

        /// 代入演算子
        Ref<void> &operator=(const Ref<void> &ref)
        {
            Ref<u8>::disconnect(mInfo, mPrev, mNext);
            mInfo = ref.mInfo;
            Ref<u8>::connect(mInfo, mPrev, mNext);
            return *this;
        }

        /// 代入演算子
        Ref<void> &operator=(Ref<void> &&ref) noexcept
        {
            Ref<u8>::disconnect(mInfo, mPrev, mNext);
            mInfo = ref.mInfo;
            Ref<u8>::connect(mInfo, mPrev, mNext);
            return *this;
        }

        /// キャストします
        template<class U>
        explicit operator Ref<U>() const
        {
            return Ref<U>(mInfo);
        }

        /// 参照先にアクセスします
        void *pointer()
        {
            return (mInfo ? (void *) mInfo->top : nullptr);
        }

        /// 参照先にアクセスします
        const void *pointer() const
        {
            return (mInfo ? (void *) mInfo->top : nullptr);
        }

        /// 参照カウントを返します
        size_t count() const
        {
            return (mInfo ? mInfo->count : 0);
        }
    };

    /// ユニーク参照クラスです
    template<class T>
    class UR
    {
        template<class T> friend class RC;
        friend class UR<void>;

        Ref<u8>::Info *mInfo; // 参照情報

        // Infoを確保します
        static Ref<u8>::Info *alloc()
        {
            return new(Memory::allocate(sizeof(Ref<u8>::Info))) Ref<u8>::Info();
        }

        // 解放します
        static void free(Ref<u8>::Info *&info)
        {
            if(info)
            {
                // Refをすべてnullptrにする
                for(auto p = info->top->mNext; p != info->top; p = p->mNext)
                {
                    p->mInfo = nullptr;
                }
                info->top->mInfo = nullptr;
                // ポインタ解放
                (*info->deleter)(info->pointer);
                // Info解放
                Memory::deallocate(info);
            }
        }

        // 移します
        template<class U>
        static void move(UR<U> &from, UR<U> &to)
        {
            free(to.mInfo);
            to.mInfo = from.mInfo;
            if(from.mInfo)
            {
                from.mInfo = nullptr;
            }
        }

    public:

        /// コンストラクタ
        UR()
            : mInfo(nullptr)
        {}

        /// コンストラクタ
        UR(T *pointer, IDeleter *deleter = Memory::deleter())
            : mInfo(alloc())
        {
            mInfo->pointer = pointer;
            mInfo->deleter = deleter;
            mInfo->count = 1;
        }

        /// ムーブコンストラクタ
        UR(UR<T> &ur)
        {
            move(ur, *this);
        }

        /// ムーブコンストラクタ
        UR(UR<T> &&ur) noexcept
        {
            move(ur, *this);
        }

        /// デストラクタ
        ~UR()
        {
            free(mInfo);
        }

        /// 代入演算子
        UR<T> &operator=(UR<T> &ur)
        {
            move(ur, *this);
            return *this;
        }

        /// 代入演算子
        UR<T> &operator=(UR<T> &&ur) noexcept
        {
            move(ur, *this);
            return *this;
        }

        /// 参照先を取得します
        T &operator*()
        {
            if(!mInfo) printError("null pointer. T &UR<T>::operator*()");
            return *(T *) mInfo->pointer;
        }

        /// 参照先を取得します
        const T &operator*() const
        {
            if(!mInfo) printError("null pointer. const T &UR<T>::operator*() const");
            return *(T *) mInfo->pointer;
        }

        /// 参照先にアクセスします
        T *operator->()
        {
            if(!mInfo) printError("null pointer. T *UR<T>::operator->()");
            return (T *) mInfo->pointer;
        }

        /// キャストします
        template<class U>
        explicit operator UR<U>() const
        {
            UR<U> ur;
            ur.mInfo = mInfo;
            if(mInfo)
            {
                mInfo = nullptr;
            }
            return ur;
        }

        /// キャストします
        operator Ref<T>() const
        {
            return Ref<T>(mInfo);
        }

        /// 参照先を取得します
        T &reference()
        {
            if(!mInfo) printError("null pointer. T &UR<T>::reference()");
            return *(T *) mInfo->pointer;
        }

        /// 参照先を取得します
        const T &reference() const
        {
            if(!mInfo) printError("null pointer. const T &UR<T>::reference() const");
            return *(T *) mInfo->pointer;
        }

        /// 参照先にアクセスします
        T *pointer()
        {
            return (mInfo ? (T *) mInfo->top : nullptr);
        }

        /// 参照先にアクセスします
        const T *pointer() const
        {
            return (mInfo ? (T *) mInfo->top : nullptr);
        }
    };

    /// ユニーク参照クラスです
    template<>
    class UR<void>
    {
        template<class T> friend class RC;

        Ref<u8>::Info *mInfo; // 参照情報

    public:


        /// コンストラクタ
        UR()
            : mInfo(nullptr)
        {}

        /// コンストラクタ
        UR(void *pointer, IDeleter *deleter = Memory::deleter())
            : mInfo(UR<u8>::alloc())
        {
            mInfo->pointer = pointer;
            mInfo->deleter = deleter;
            mInfo->count = 1;
        }

        /// ムーブコンストラクタ
        UR(UR<void> &ur)
        {
            UR<u8>::move(ur, *this);
        }

        /// ムーブコンストラクタ
        UR(UR<void> &&ur) noexcept
        {
            UR<u8>::move(ur, *this);
        }

        /// デストラクタ
        ~UR()
        {
            UR<u8>::free(mInfo);
        }

        /// 代入演算子
        UR<void> &operator=(UR<void> &ur)
        {
            UR<u8>::move(ur, *this);
            return *this;
        }

        /// 代入演算子
        UR<void> &operator=(UR<void> &&ur) noexcept
        {
            UR<u8>::move(ur, *this);
            return *this;
        }

        /// キャストします
        template<class U>
        explicit operator UR<U>() const
        {
            UR<U> ur;
            ur.mInfo = mInfo;
            if(mInfo)
            {
                mInfo = nullptr;
            }
            return ur;
        }

        /// キャストします
        operator Ref<void>() const
        {
            return Ref<void>(mInfo);
        }

        /// 参照先にアクセスします
        void *pointer()
        {
            return (mInfo ? (void *) mInfo->top : nullptr);
        }

        /// 参照先にアクセスします
        const void *pointer() const
        {
            return (mInfo ? (void *) mInfo->top : nullptr);
        }
    };

    /// ユニーク参照ポインタを作成します
    template<class T, class...Args>
    UR<T> newUR(Args...args)
    {
        auto ptr = new(Memory::allocate(sizeof(T))) T(args...);
        return UR(ptr, Memory::deleter());
    }

    /// 参照カウンタクラスです
    template<class T>
    class RC
    {
        template<class T> friend class UR;
        friend class RC<void>;

        Ref<u8>::Info *mInfo; // 参照情報

        // Infoを確保します
        static Ref<u8>::Info *alloc()
        {
            return UR<u8>::alloc();
        }

        // 解放します
        static void free(Ref<u8>::Info *&info)
        {
            if(info)
            {
                info->count -= 1;
                if(!info->count)
                {
                    UR<u8>::free(info);
                }
            }
        }

        // コピーします
        template<class U>
        static void copy(const RC<U> &from, RC<U> &to)
        {
            free(to.mInfo);
            to.mInfo = from.mInfo;
            if(to.mInfo)
            {
                to.mInfo->count += 1;
            }
        }

    public:

        /// コンストラクタ
        RC()
            : mInfo(nullptr)
        {}

        /// コンストラクタ
        RC(T *pointer, IDeleter *deleter = Memory::deleter())
            : mInfo(alloc())
        {
            mInfo->pointer = pointer;
            mInfo->deleter = deleter;
            mInfo->count = 1;
        }

        /// コピーコンストラクタ
        RC(const RC<T> &rc)
        {
            copy(rc, *this);
        }

        /// ムーブコンストラクタ
        RC(RC<T> &&rc) noexcept
        {
            copy(rc, *this);
        }

        /// デストラクタ
        ~RC()
        {
            free(mInfo);
        }

        /// 代入演算子
        RC<T> &operator=(const RC<T> &rc)
        {
            copy(rc, *this);
            return *this;
        }

        /// 代入演算子
        RC<T> &operator=(RC<T> &&rc) noexcept
        {
            copy(rc, *this);
            return *this;
        }

        /// 参照先を取得します
        T &operator*()
        {
            if(!mInfo) printError("null pointer. T &RC<T>::operator*()");
            return *(T *) mInfo->pointer;
        }

        /// 参照先を取得します
        const T &operator*() const
        {
            if(!mInfo) printError("null pointer. const T &RC<T>::operator*() const");
            return *(T *) mInfo->pointer;
        }

        /// 参照先にアクセスします
        T *operator->()
        {
            if(!mInfo) printError("null pointer. T *RC<T>::operator->()");
            return (T *) mInfo->pointer;
        }

        /// キャストします
        template<class U>
        explicit operator RC<U>() const
        {
            RC<U> rc;
            rc.mInfo = mInfo;
            if(mInfo)
            {
                mInfo->count += 1;
            }
            return rc;
        }

        /// キャストします
        operator Ref<T>() const
        {
            return Ref<T>(mInfo);
        }

        /// 参照先を取得します
        T &reference()
        {
            if(!mInfo) printError("null pointer. T &RC<T>::reference()");
            return *(T *) mInfo->pointer;
        }

        /// 参照先を取得します
        const T &reference() const
        {
            if(!mInfo) printError("null pointer. const T &RC<T>::reference() const");
            return *(T *) mInfo->pointer;
        }

        /// 参照先にアクセスします
        T *pointer()
        {
            return (mInfo ? (T *) mInfo->top : nullptr);
        }

        /// 参照先にアクセスします
        const T *pointer() const
        {
            return (mInfo ? (T *) mInfo->top : nullptr);
        }

        /// 参照カウントを返します
        size_t count() const
        {
            return (mInfo ? mInfo->count : 0);
        }
    };

    /// 参照カウンタクラスです
    template<>
    class RC<void>
    {
        template<class T> friend class UR;

        Ref<u8>::Info *mInfo; // 参照情報

    public:

        /// コンストラクタ
        RC()
            : mInfo(nullptr)
        {}

        /// コンストラクタ
        RC(void *pointer, IDeleter *deleter = Memory::deleter())
            : mInfo(RC<u8>::alloc())
        {
            mInfo->pointer = pointer;
            mInfo->deleter = deleter;
            mInfo->count = 1;
        }

        /// コピーコンストラクタ
        RC(const RC<void> &rc)
        {
            RC<u8>::copy(rc, *this);
        }

        /// ムーブコンストラクタ
        RC(RC<void> &&rc) noexcept
        {
            RC<u8>::copy(rc, *this);
        }

        /// デストラクタ
        ~RC()
        {
            free(mInfo);
        }

        /// 代入演算子
        RC<void> &operator=(const RC<void> &rc)
        {
            RC<u8>::copy(rc, *this);
            return *this;
        }

        /// 代入演算子
        RC<void> &operator=(RC<void> &&rc) noexcept
        {
            RC<u8>::copy(rc, *this);
            return *this;
        }

        /// キャストします
        template<class U>
        explicit operator RC<U>() const
        {
            RC<U> rc;
            rc.mInfo = mInfo;
            if(mInfo)
            {
                mInfo->count += 1;
            }
            return rc;
        }

        /// キャストします
        operator Ref<void>() const
        {
            return Ref<void>(mInfo);
        }
        
        /// 参照先にアクセスします
        void *pointer()
        {
            return (mInfo ? (void *) mInfo->top : nullptr);
        }

        /// 参照先にアクセスします
        const void *pointer() const
        {
            return (mInfo ? (void *) mInfo->top : nullptr);
        }

        /// 参照カウントを返します
        size_t count() const
        {
            return (mInfo ? mInfo->count : 0);
        }
    };

    /// 参照カウンタポインタを作成します
    template<class T, class...Args>
    RC<T> newRC(Args...args)
    {
        auto ptr = new(Memory::allocate(sizeof(T))) T(args...);
        return RC(ptr, Memory::deleter());
    }
}

#endif // !ELEKICORE_POINTER_HPP