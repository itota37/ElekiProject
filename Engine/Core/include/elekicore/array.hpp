/// @file array.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// ポインタイテレータ、静的配列、動的配列を提供します

#ifndef ELEKICORE_ARRAY_HPP
#define ELEKICORE_ARRAY_HPP

#include <algorithm>
#include "functional.hpp"
#include "allocation.hpp"
#include "datalog.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{
    
    /// 生配列を整列させます
    /// @param elements 整列させる生配列
    /// @param count 整列させる要素数
    /// @param trueLR 先頭から第1引数、第2引数の順に並べる場合、真を返してください
    template<class T>
    void sort(T *elements, size_t count, const Compare<T> &trueLR = [](const T &l, const T &r) { return l <= r; })
    {
        std::sort(elements, &elements[count], trueLR);
    }

    /// 生配列をコピーします 
    /// 生配列の為、オーバーランが発生する可能性があります
    /// @param from コピー元
    /// @param fromBegin コピー元開始位置
    /// @param to コピー先
    /// @param toBegin コピー先開始位置
    /// @param count コピーする要素数
    template<class T>
    void copy(const T *from, size_t fromBegin, T *to, size_t toBegin, size_t count)
    {
        for(size_t i = 0, f = fromBegin, t = toBegin; i < count; i++, f++, t++)
        {
            to[t] = from[f];
        }
    }

    /// ポインタイテレータ
    template<class T>
    class PointerItr
    {
    public:

        using difference_type = ptrdiff_t;                         ///< 符号付き整数型
        using value_type = T;                                      ///< 要素の型
        using pointer = T *;                                       ///< 要素のポインタ型
        using reference = T &;                                     ///< 要素の参照型
        using iterator_category = std::random_access_iterator_tag; ///< イテレータの種類

    private:

        T *mElement;     // 要素ポインタ

    public:

        /// コンストラクタ
        PointerItr()
            : mElement(nullptr)
        {}

        /// コンストラクタ
        /// @param element 要素ポインタ
        PointerItr(T *element)
            : mElement(element)
        {}

        /// コピーコンストラクタ
        /// @param itr コピー元
        PointerItr(const PointerItr<T> &itr)
            : mElement(itr.mElement)
        {}

        /// ムーブコンストラクタ
        /// @param itr コピー元
        PointerItr(PointerItr<T> &&itr)
            : mElement(itr.mElement)
        {}

        /// コピー代入します
        /// @param r 左辺
        PointerItr<T> &operator=(const PointerItr<T> &itr)
        {
            mElement = itr.mElement;
            return *this;
        }

        /// ムーブ代入します
        /// @param r 左辺
        PointerItr<T> &operator=(PointerItr<T> &&itr)
        {
            mElement = itr.mElement;
            return *this;
        }

        /// 次の方向に指定分移動します
        /// @param r 移動する差分
        PointerItr<T> &operator+=(difference_type r)
        {
            mElement += r;
            return *this;
        }

        /// 前の方向に指定分移動します
        /// @param r 移動する差分
        PointerItr<T> &operator-=(difference_type r)
        {
            mElement -= r;
            return *this;
        }

        /// 次に移動します
        PointerItr<T> &operator++()
        {
            mElement++;
            return *this;
        }

        /// 次に移動します
        /// @return 移動前のイテレータ
        PointerItr<T> operator++(int)
        {
            auto copy = *this;
            mElement++;
            return copy;
        }

        /// 前に移動します
        PointerItr<T> &operator--()
        {
            mElement--;
            return *this;
        }

        /// 前に移動します
        /// @return 移動前のイテレータ
        PointerItr<T> operator--(int)
        {
            auto copy = *this;
            mElement--;
            return copy;
        }

        /// イテレータ間の差を求めます
        difference_type operator-(const PointerItr<T> &itr) const
        {
            return (difference_type) mElement - (difference_type) itr.mElement;
        }

        /// イテレータ間の差を求めます
        difference_type operator-(PointerItr<T> &&itr) const
        {
            return (difference_type) mElement - (difference_type) itr.mElement;
        }

        /// 要素にアクセスします
        /// @return 要素
        reference operator*()
        {
            return *mElement;
        }

        /// 要素のメンバにアクセスします
        /// @return 要素のメンバ
        pointer operator->()
        {
            return mElement;
        }

        /// 位置が等しいか判定します
        /// @param r 右辺
        /// @return 位置が等しい場合真
        bool operator==(const PointerItr<T> &r) const
        {
            return mElement == r.mElement;
        }

        /// 位置が等しいか判定します
        /// @param r 右辺
        /// @return 位置が等しい場合真
        bool operator==(PointerItr<T> &&r) const
        {
            return mElement == r.mElement;
        }

        /// 位置が等しいくないか判定します
        /// @param r 右辺
        /// @return 位置が等しいくない場合真
        bool operator!=(const PointerItr<T> &r) const
        {
            return !(*this == r);
        }

        /// 位置が等しいくないか判定します
        /// @param r 右辺
        /// @return 位置が等しいくない場合真
        bool operator!=(PointerItr<T> &&r) const
        {
            return !(*this == r);
        }
    };

    /// 不変ポインタイテレータ
    template<class T>
    class ConstPointerItr
    {
    public:

        using difference_type = ptrdiff_t;                         ///< 符号付き整数型
        using value_type = const T;                                ///< 要素の型
        using pointer = const T *;                                 ///< 要素のポインタ型
        using reference = const T &;                               ///< 要素の参照型
        using iterator_category = std::random_access_iterator_tag; ///< イテレータの種類

    private:

        const T *mElement;     // 要素ポインタ

    public:

        /// コンストラクタ
        ConstPointerItr()
            : mElement(nullptr)
        {}

        /// コンストラクタ
        /// @param element 要素ポインタ
        ConstPointerItr(const T *element)
            : mElement(element)
        {}

        /// コピーコンストラクタ
        /// @param itr コピー元
        ConstPointerItr(const ConstPointerItr<T> &itr)
            : mElement(itr.mElement)
        {}

        /// ムーブコンストラクタ
        /// @param itr コピー元
        ConstPointerItr(ConstPointerItr<T> &&itr)
            : mElement(itr.mElement)
        {}

        /// コピー代入します
        /// @param r 左辺
        ConstPointerItr<T> &operator=(const ConstPointerItr<T> &itr)
        {
            mElement = itr.mElement;
            return *this;
        }

        /// ムーブ代入します
        /// @param r 左辺
        ConstPointerItr<T> &operator=(ConstPointerItr<T> &&itr)
        {
            mElement = itr.mElement;
            return *this;
        }

        /// 次の方向に指定分移動します
        /// @param r 移動する差分
        ConstPointerItr<T> &operator+=(difference_type r)
        {
            mElement += r;
            return *this;
        }

        /// 前の方向に指定分移動します
        /// @param r 移動する差分
        ConstPointerItr<T> &operator-=(difference_type r)
        {
            mElement -= r;
            return *this;
        }

        /// 次に移動します
        ConstPointerItr<T> &operator++()
        {
            mElement++;
            return *this;
        }

        /// 次に移動します
        /// @return 移動前のイテレータ
        ConstPointerItr<T> operator++(int)
        {
            auto copy = *this;
            mElement++;
            return copy;
        }

        /// 前に移動します
        ConstPointerItr<T> &operator--()
        {
            mElement--;
            return *this;
        }

        /// 前に移動します
        /// @return 移動前のイテレータ
        ConstPointerItr<T> operator--(int)
        {
            auto copy = *this;
            mElement--;
            return copy;
        }

        /// イテレータ間の差を求めます
        difference_type operator-(const ConstPointerItr<T> &itr) const
        {
            return (difference_type) mElement - (difference_type) itr.mElement;
        }

        /// イテレータ間の差を求めます
        difference_type operator-(ConstPointerItr<T> &&itr) const
        {
            return (difference_type) mElement - (difference_type) itr.mElement;
        }

        /// 要素にアクセスします
        /// @return 要素
        reference operator*() const
        {
            return *mElement;
        }

        /// 要素のメンバにアクセスします
        /// @return 要素のメンバ
        pointer operator->() const
        {
            return mElement;
        }

        /// 位置が等しいか判定します
        /// @param r 右辺
        /// @return 位置が等しい場合真
        bool operator==(const ConstPointerItr<T> &r) const
        {
            return mElement == r.mElement;
        }

        /// 位置が等しいか判定します
        /// @param r 右辺
        /// @return 位置が等しい場合真
        bool operator==(ConstPointerItr<T> &&r) const
        {
            return mElement == r.mElement;
        }

        /// 位置が等しいくないか判定します
        /// @param r 右辺
        /// @return 位置が等しいくない場合真
        bool operator!=(const ConstPointerItr<T> &r) const
        {
            return !(*this == r);
        }

        /// 位置が等しいくないか判定します
        /// @param r 右辺
        /// @return 位置が等しいくない場合真
        bool operator!=(ConstPointerItr<T> &&r) const
        {
            return !(*this == r);
        }
    };

    /// 静的配列を提供します
    template<class T, size_t N>
    class Array
    {
    public:

        T elements[N]; ///< 要素配列

        /// 添え字から要素にアクセスします
        T &operator[](size_t index)
        {
            if(index >= N) printError(T("out of range. T &Array<T>::operator[](size_t index)"));
            return elements[index];
        }

        /// 添え字から要素にアクセスします
        const T &operator[](size_t index) const
        {
            if(index >= N) printError(T("out of range. const T &Array<T>::operator[](size_t index) const"));
            return elements[index];
        }

        /// 添え字から要素にアクセスします
        T &at(size_t index)
        {
            if(index >= N) printError(T("out of range. T &Array<T>::at(size_t index)"));
            return elements[index];
        }

        /// 添え字から要素にアクセスします
        const T &at(size_t index) const
        {
            if(index >= N) printError(T("out of range. const T &Array<T>::at(size_t index) const"));
            return elements[index];
        }

        /// 要素数を返します
        constexpr size_t count() const
        {
            return N;
        }

        /// 整列します
        /// @param trueLR 先頭から第1引数、第2引数の順に並べる場合、真を返してください
        Array<T, N> &sort(const Compare<T> &trueLR = [](const T &l, const T &r) { return l <= r; })
        {
            sort<T>(elements, N, trueLR);
            return *this;
        }

        /// コピー配列を返します
        /// @tparam M コピー配列の要素数
        /// @tparam B コピーする範囲の開始位置
        /// @tparam C コピーする範囲の要素数
        template<size_t M, size_t B = 0, size_t C = M>
        Array<T, M> copy() const
        {
            Array<T, M> arr;
            auto len = N - B < C ? N - B : C;
            copy<T>(elements, B, arr.elements, 0, len);
            return arr;
        }

        /// コピー配列を返します
        /// @tparam M コピー配列の要素数
        /// @param beginIndex コピーする範囲の開始位置
        /// @param rangeCount コピーする範囲の要素数
        template<size_t M>
        Array<T, M> copy(size_t beginIndex, size_t rangeCount) const
        {
            Array<T, M> arr;
            auto len = N - beginIndex < rangeCount ? N - beginIndex : rangeCount;
            copy<T>(elements, beginIndex, arr.elements, 0, len);
            return arr;
        }

        /// 先頭イテレータを返します
        PointerItr<T> begin()
        {
            return PointerItr<T>(elements);
        }

        /// 先頭イテレータを返します
        ConstPointerItr<T> begin() const
        {
            return ConstPointerItr<T>(elements);
        }

        /// 番兵イテレータを返します
        PointerItr<T> end()
        {
            return PointerItr<T>(&elements[N]);
        }

        /// 番兵イテレータを返します
        ConstPointerItr<T> end() const
        {
            return ConstPointerItr<T>(&elements[N]);
        }
    };

    /// 連結します
    template<class T, size_t N, size_t M>
    Array<T, N + M> operator+(const Array<T, N> &l, const Array<T, M> &r)
    {
        Array<T, N + M> ret;
        for(size_t i = 0; i < N; i++)
        {
            ret[i] = l[i];
        }
        for(size_t i = N, j = 0; j < M; i++, j++)
        {
            ret[i] = r[j];
        }
        return ret;
    }


    /// 動的配列を提供します
    template<class T>
    class List
    {
        static constexpr size_t INIT_ELEM_CNT_A = 8;   // 初期の配列長の有余
        static constexpr size_t TYPE_SIZE = sizeof(T); // 1データサイズ
        
        IAllocator *mAllocator;                        // アロケータ
        size_t mSize;                                  // 配列サイズ
        size_t mCount;                                 // 要素数
        T *mElements;                                  // 要素配列

        // コピー
        void copyFrom(T *ptr, size_t count)
        {
            mAllocator->deallocate(mElements, mSize);
            mCount = count;
            mSize = mCount * 2;
            mElements = mAllocator->allocate(mSize);

            copy(ptr, 0, mElements, 0, mCount);
        }

        // 末尾にリスト追加
        void addFrom(T *ptr, size_t count)
        {
            auto newCount = mCount + count;
            auto newSize = newCount * 2;
            auto newElems = mAllocator->allocate(newSize);

            copy(mElements, 0, newElems, 0, mCount);
            copy(ptr, 0, newElems, mCount, count);

            mAllocator->deallocate(mElements, mSize);
            mCount = newCount;
            mSize = newSize;
            mElements = newElems;
        }

        // 末尾に要素を追加
        void addFrom(const T &elem)
        {
            // 配列が足りない場合は倍に増やす
            if(mCount == mSize)
            {
                auto newSize = mCount * 2;
                auto newElems = mAllocator->allocate(newSize);

                copy(mElements, 0, newElems, 0, mCount);

                mAllocator->deallocate(mElements, mSize);
                mSize = newSize;
                mElements = newElems;
            }

            // 追加
            mElements[mCount] = elem;
            mCount++;
        }

        // 指定位置にリスト追加
        void insertFrom(size_t index, T *ptr, size_t count)
        {
            auto newCount = mCount + count;
            auto newSize = newCount * 2;
            auto newElems = mAllocator->allocate(newSize);

            copy(mElements, 0, newElems, 0, index);
            copy(ptr, 0, newElems, index, count);
            copy(mSize, index, newElems, index + count, mCount - index);

            mAllocator->deallocate(mElements, mSize);
            mCount = newCount;
            mSize = newSize;
            mElements = newElems;
        }

        // 指定位置に要素を追加
        void insertFrom(size_t index, const T &elem)
        {
            // 配列が足りない場合は倍に増やす
            if(mCount == mSize)
            {
                auto newSize = mCount * 2;
                auto newElems = mAllocator->allocate(newSize);

                copy(mElements, 0, newElems, 0, index);
                copy(mElements, index, newElems, index + 1, mCount - index);

                mAllocator->deallocate(mElements, mSize);
                mSize = newSize;
                mElements = newElems;

                // 追加
                mElements[index] = elem;
                mCount++;
            }
            else
            {
                // 移動
                for(int i = mCount - 1; i >= index; i--)
                {
                    mElements[i + 1] = mElements[i];
                }

                // 追加
                mElements[index] = elem;
                mCount++;
            }
        }

    public:

        /// コンストラクタ
        /// @param count 初期要素数
        /// @param allocator 使用するアロケータ
        List(size_t count, IAllocator *allocator = Memory::allocator())
            : mAllocator(allocator)
            , mSize(count + INIT_ELEM_CNT_A)
            , mCount(count)
            , mElements(allocator.allocate(mSize))
        {}

        /// コンストラクタ
        /// @param allocator 使用するアロケータ
        List(IAllocator *allocator = Memory::allocator())
            : List(0, allocator)
        {}

        /// コンストラクタ
        /// @param list 初期化リスト
        /// @param allocator 使用するアロケータ
        List(std::initializer_list<T> list, IAllocator *allocator = Memory::allocator())
            : List(list.size(), allocator)
        {}

        /// コピーコンストラクタ
        List(const List<T> &list)
            : List(list.count(), list.mAllocator)
        {
            copy(list.mElements, 0, mElements, 0, mCount);
        }

        /// ムーブコンストラクタ
        List(List<T> &&list) noexcept
            : List(list.count(), list.mAllocator)
        {
            copy(list.mElements, 0, mElements, 0, mCount);
        }

        /// デストラクタ
        ~List()
        {
            mAllocator->deallocate(mElements, mSize);
        }

        /// コピー代入します
        List<T> &operator=(const List<T> &list)
        {
            copyFrom(list.mElements, list.mCount);
            return *this;
        }

        /// ムーブ代入します
        List<T> &operator=(List<T> &&list) noexcept
        {
            copyFrom(list.mElements, list.mCount);
            return *this;
        }

        /// 末尾に追加します
        List<T> &operator+=(const List<T> &list)
        {
            addFrom(list.mElements, list.mSize);
            return *this;
        }

        /// 末尾に追加します
        List<T> &operator+=(List<T> &&list) noexcept
        {
            addFrom(list.mElements, list.mSize);
            return *this;
        }

        /// 末尾に追加します
        List<T> &operator+=(const T &element)
        {
            addFrom(element);
            return *this;
        }

        /// 末尾に追加します
        List<T> &operator+=(T &&element) noexcept
        {
            addFrom(element);
            return *this;
        }

        /// 添え字から要素にアクセスします
        T &operator[](size_t index)
        {
            if(index >= mCount) printError(T("out of range. T &List<T>::operator[](size_t index)"));
            return mElements[index];
        }

        /// 添え字から要素にアクセスします
        const T &operator[](size_t index) const
        {
            if(index >= mCount) printError(T("out of range. const T &List<T>::operator[](size_t index) const"));
            return mElements[index];
        }

        /// 配列のサイズを変更します
        List<T> &resize(size_t size)
        {
            // コピー
            auto newElems = mAllocator->allocate(size);
            auto len = mCount < size ? mCount : size;
            copy(mElements, 0, newElems, 0, len);

            mAllocator->deallocate(mElements, mSize);
            mElements = newElems;
            mSize = size;
            mCount = len;

            return *this;
        }

        /// 末尾に追加します
        List<T> &add(const List<T> &list)
        {
            addFrom(list.mElements, list.mSize);
            return *this;
        }

        /// 末尾に追加します
        List<T> &add(List<T> &&list) noexcept
        {
            addFrom(list.mElements, list.mSize);
            return *this;
        }

        /// 末尾に追加します
        List<T> &add(const T &element)
        {
            addFrom(element);
            return *this;
        }

        /// 末尾に追加します
        List<T> &add(T &&element) noexcept
        {
            addFrom(element);
            return *this;
        }

        /// 添え字から要素にアクセスします
        T &at(size_t index)
        {
            if(index >= mCount) printError(T("out of range. T &List<T>::at(size_t index)"));
            return mElements[index];
        }

        /// 添え字から要素にアクセスします
        const T &at(size_t index) const
        {
            if(index >= mCount) printError(T("out of range. const T &at(size_t index) const"));
            return mElements[index];
        }

        /// 指定位置に挿入します
        List<T> &insert(size_t index, const List<T> &list)
        {
            if(index >= mCount)
            {
                printError(T("out of range. List<T> &List<T>::insert(size_t index, const List<T> &list)"));
            }
            else
            {
                insertFrom(index, list.mElements, list.mCount);
            }
            return *this;
        }

        /// 指定位置に挿入します
        List<T> &insert(size_t index, List<T> &&list)
        {
            if(index >= mCount)
            {
                printError(T("out of range. List<T> &List<T>::insert(size_t index, List<T> &&list)"));
            }
            else
            {
                insertFrom(index, list.mElements, list.mCount);
            }
            return *this;
        }

        /// 指定位置に挿入します
        List<T> &insert(size_t index, const T &element)
        {
            if(index >= mCount)
            {
                printError(T("out of range. List<T> &List<T>::insert(size_t index, const T &element)"));
            }
            else
            {
                insertFrom(index, element);
            }
            return *this;
        }

        /// 指定位置に挿入します
        List<T> &insert(size_t index, T &&element)
        {
            if(index >= mCount)
            {
                printError(T("out of range. List<T> &List<T>::insert(size_t index, T &&element)"));
            }
            else
            {
                insertFrom(index, element);
            }
            return *this;
        }

        /// 一致する要素の位置を返します
        /// @param element 検索する要素
        /// @param equal 一致するとき真を返す関数オブジェクト
        /// @param invers 真のとき末尾から、偽のとき先頭から走査します
        /// @return 一致した要素の位置
        /// @retval 要素数 一致する要素がありませんでした
        size_t indexOf(const T &element, const Compare<T> &equal = [](const T &l, const T &r) { return l == r; }, bool invers = false)
        {
            if(!invers)
            {
                for(size_t i = 0; i < mCount; i++)
                {
                    if(equal(element, mElements[i])) return i;
                }
            }
            else
            {
                for(int i = mCount - 1; i >= 0; i--)
                {
                    if(equal(element, mElements[i])) return i;
                }
            }
            return mCount;
        }

        /// 一致する要素の位置を返します
        /// @param element 検索する要素
        /// @param equal 一致するとき真を返す関数オブジェクト
        /// @param invers 真のとき末尾から、偽のとき先頭から走査します
        /// @return 一致した要素の位置
        /// @retval 要素数 一致する要素がありませんでした
        size_t indexOf(T &&element, const Compare<T> &equal = [](const T &l, const T &r) { return l == r; }, bool invers = false)
        {
            T &tmp = element;
            return indexOf(tmp, equal, invers);
        }

        /// 指定位置を削除します
        /// @param index 指定位置
        /// @param swapLast 真のとき指定位置に末尾を入れます、偽のときは詰めます
        List<T> &removeAt(size_t index, bool swapLast = false)
        {
            if(index >= mCount)
            {
                printError(T("out of range. List<T> &List<T>::removeAt(size_t index, bool swapLast)"));
            }
            else
            {
                if(swapLast)
                {
                    // 末尾を代わりに入れる
                    mElements[index] = mElements[mCount - 1];
                }
                else
                {
                    // 順に詰める
                    for(size_t i = index; i < mCount - 1; i++)
                    {
                        mElements[i] = mElements[i + 1];
                    }
                }
                mCount--;

                // 要素数が配列長の1/2以下のとき、配列長を3/4にする
                if(mCount <= (size_t) (mSize * 0.5f))
                {
                    resize((size_t) (mSize * 0.75f)));
                }
            }
            return *this;
        }

        /// はじめに一致した要素を削除します
        /// @param element 削除する要素
        /// @param equal 一致するとき真を返す関数オブジェクト
        /// @param invers 真のとき末尾から、偽のとき先頭から走査します
        /// @param swapLast 真のとき指定位置に末尾を入れます、偽のときは詰めます
        List<T> &remove(const T &element, const Compare<T> &equal = [](const T &l, const T &r) { return l == r; }, bool invers = false, bool swapLast = false)
        {
            size_t index = indexOf(element, equal, invers);
            if(index != mCount) removeAt(index, swapLast);
            return *this;
        }

        /// はじめに一致した要素を削除します
        /// @param element 削除する要素
        /// @param equal 一致するとき真を返す関数オブジェクト
        /// @param invers 真のとき末尾から、偽のとき先頭から走査します
        /// @param swapLast 真のとき指定位置に末尾を入れます、偽のときは詰めます
        List<T> &remove(T &&element, const Compare &equal = [](const T &l, const T &r) { return l == r; }, bool invers = false, bool swapLast = false)
        {
            size_t index = indexOf(element, equal, invers);
            if(index != mCount) removeAt(index, swapLast);
            return *this;
        }

        /// 要素数を返します
        size_t count() const
        {
            return mCount;
        }

        /// 整列します
        /// @param trueLR 先頭から第1引数、第2引数の順に並べる場合、真を返してください
        List<T> &sort(const Compare<T> &trueLR = [](const T &l, const T &r) { return l <= r; })
        {
            sort<T>(mElements, mCount, trueLR);
            return *this;
        }

        /// クリアします
        void clear()
        {
            mAllocator->deallocate(mElements, mSize);
            mSize = INIT_ELEM_CNT_A;
            mCount = 0;
            mElements = mAllocator->allocate(mSize);
        }

        /// 先頭イテレータを返します
        PointerItr<T> begin()
        {
            return PointerItr<T>(mElements);
        }

        /// 先頭イテレータを返します
        ConstPointerItr<T> begin() const
        {
            return ConstPointerItr<T>(mElements);
        }

        /// 番兵イテレータを返します
        PointerItr<T> end()
        {
            return PointerItr<T>(&mElements[mCount]);
        }

        /// 番兵イテレータを返します
        ConstPointerItr<T> end() const
        {
            return ConstPointerItr<T>(&mElements[mCount]);
        }
    };

    /// 連結します
    template<class T>
    List<T> operator+(const List<T> &l, const List<T> &r)
    {
        return List<T>(l) += r;
    }

}

#endif // !ELEKICORE_ARRAY_HPP