/// @file set.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// 集合を提供します

#ifndef ELEKICORE_SET_HPP
#define ELEKICORE_SET_HPP

#include "hash.hpp"
#include "array.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{

    /// 等しいか判定する特殊化構造体です
    template<class T>
    struct EqualTo
    {
        /// 等しいか判定します
        bool operator()(const T &l, const T &r)
        {
            return l == r;
        }
    };

    /// 集合を提供します
    template<class T, class H = Hash<T>, class E = EqualTo<T>>
    class Set
    {
        static constexpr size_t NONE_INDEX_NUM = std::numeric_limits<size_t>::max();
        static constexpr size_t INIT_ELEM_CNT_A = 8;

        List<T> mElements;       // 要素配列
        List<size_t> mIndexes;   // 添え字配列

        // ハッシュ値から空きの位置を見つける
        size_t noneIndexOf(const T &element, const List<size_t> &indexes) const
        {
            size_t index = H{}(element) % indexes.count();
            for(size_t i = index; i < indexes.count(); i++)
            {
                if(indexes[i] == NONE_INDEX_NUM) return i;
            }
            for(size_t i = 0; i < index; i++)
            {
                if(indexes[i] == NONE_INDEX_NUM) return i;
            }
            return NONE_INDEX_NUM;
        }

        // ハッシュ値から要素を見つける
        size_t elementIndexOf(const T &element, const List<T> &elements, const List<size_t> &indexes) const
        {
            size_t index = H{}(element) % indexes.count();
            for(size_t i = index; i < indexes.count(); i++)
            {
                if(indexes[i] == NONE_INDEX_NUM && E{}(elements[indexes[i]], element)) return i;
            }
            for(size_t i = 0; i < index; i++)
            {
                if(indexes[i] == NONE_INDEX_NUM && E{}(elements[indexes[i]], element)) return i;
            }
            return NONE_INDEX_NUM;
        }

        // 要素のコピー
        void copyElements(const List<T> &fromElements, List<T> &toElements, List<size_t> newIndexes)
        {
            newIndexes.clear();
            toElements.clear();
            for(size_t i = 0; i < fromElements.count(); i++)
            {
                newIndexes.add(NONE_INDEX_NUM);
            }
            for(size_t i = 0; i < fromElements.count(); i++)
            {
                toElements[i] = fromElements[i];
                size_t index = noneIndexOf(toElements[i], newIndexes);
                newIndexes[index] = i;
            }
        }

        // コピー
        void copyFrom(const List<T> &fromElements)
        {
            // クリア
            mElements.clear();
            mIndexes.clear();

            // コピー元の2倍になるようにリサイズ
            size_t len = fromElements.count() * 2;
            mElements.resize(len);
            mIndexes.resize(len);

            // 添え字配列の初期化
            for(size_t i = 0; i < len; i++)
            {
                mIndexes.add(NONE_INDEX_NUM);
            }

            // 要素のコピー＆添え字の設定
            for(size_t i = 0; i < fromElements.count(); i++)
            {
                mElements.add(fromElements[i]);
                mIndexes[noneIndexOf(fromElements[i], mIndexes)] = i;
            }
        }

        // 添え字配列リサイズ
        void resizeIndexes(size_t newSize)
        {
            // 要素が添え字よりも多くなる場合、リサイズしない
            if(mElements.count() > newSize) return;

            // リサイズ
            mIndexes.clear();
            mIndexes.resize(newSize);

            // 初期化
            for(size_t i = 0; i < newSize; i++)
            {
                mIndexes.add(NONE_INDEX_NUM);
            }

            // 再設定
            for(size_t i = 0; i < mElements.count(); i++)
            {
                mIndexes[noneIndexOf(mElements[i], mIndexes)] = i;
            }

        }

        // 追加
        void addElement(const T &element)
        {
            // 重複を確認
            if(NONE_INDEX_NUM != elementIndexOf(element, mElements, mIndexes)) return;

            // 添え字配列が満杯の場合、2倍に増やす
            if(mElements.count() == mIndexes.count())
            {
                resizeIndexes(mIndexes.count() * 2);
            }

            // 要素配列の末尾の添え字を添え字配列に追加
            mIndexes[noneIndexOf(element, mIndexes)] = mElements.count();

            // 末尾に追加
            mElements.add(element);
        }

        // 削除
        void removeElement(const T &element)
        {
            // 要素配列が空ならば何もしない
            if(mElements.count() == 0) return;

            // 削除する要素の添え字と末尾要素の添え字を取得
            auto removeIndex = elementIndexOf(element, mElements, mIndexes);
            auto lastIndex = elementIndexOf(mElements[mElements.count() - 1]);

            // 削除位置に末尾をスワップ＆スワップ後末尾を削除
            mIndexes[lastIndex] = mIndexes[removeIndex];
            mElements.removeAt(mIndexes[removeIndex], true);
            mIndexes[removeIndex] = NONE_INDEX_NUM;

            // 要素数が配列長の半分以下の場合3/4に減らす
            if(mElements.count() <= ((size_t) ((f32) mIndexes.count() * 0.5f)))
            {
                resizeIndexes((size_t) ((f32) mIndexes.count() * 0.75f));
            }
        }

        // 重複
        template<class I, class F>
        void leaveDuplicate(const Set<T, I, F> &set)
        {
            for(size_t i = 0; i < mElements.count();)
            {
                if(NONE_INDEX_NUM == elementIndexOf(mElements[i], set.mElements, set.mIndexes))
                {
                    removeElement(mElements[i]);
                }
                else
                {
                    i++;
                }
            }
        }

    public:

        /// コンストラクタ
        /// @param allocator アロケータ
        Set(IAllocator *allocator = Memory::allocator())
            : mElements(INIT_ELEM_CNT_A, allocator)
            , mIndexes(INIT_ELEM_CNT_A, allocator)
        {
            for(size_t i = 0; i < INIT_ELEM_CNT_A; i++)
            {
                mIndexes.add(NONE_INDEX_NUM);
            }
        }

        /// コピーコンストラクタ
        template<class I, class F>
        Set(const Set<T, I, F> &set)
            : mElements(set.mElements)
            , mIndexes(set.mIndexes)
        {}

        /// ムーブコンストラクタ
        template<class I, class F>
        Set(Set<T, I, F> &&set)
            : mElements(set.mElements)
            , mIndexes(set.mIndexes)
        {}

        /// デストラクタ
        ~Set()
        {}

        /// 代入します
        template<class I, class F>
        Set<T, H, E> &operator=(const Set<T, I, F> &r)
        {
            copyFrom(r.mElements);
            return *this;
        }

        /// 代入します
        template<class I, class F>
        Set<T, H, E> &operator=(Set<T, I, F> &&r) noexcept
        {
            copyFrom(r.mElements);
            return *this;
        }

        /// 追加します
        Set<T, H, E> &operator|=(const T &r)
        {
            addElement(r);
            return *this;
        }

        /// 追加します
        Set<T, H, E> &operator|=(T &&r)
        {
            addElement(r);
            return *this;
        }

        /// 削除します
        Set<T, H, E> &operator-=(const T &r)
        {
            removeElement(r);
            return *this;
        }

        /// 削除します
        Set<T, H, E> &operator-=(T &&r)
        {
            removeElement(r);
            return *this;
        }

        /// 和集合で結合します
        template<class I, class F>
        Set<T, H, E> &operator|=(const Set<T, I, F> &r)
        {
            for(size_t i = 0; i < r.mElements.count(); i++)
            {
                addElement(r.mElements[i]);
            }
            return *this;
        }

        /// 和集合で結合します
        template<class I, class F>
        Set<T, H, E> &operator|=(Set<T, I, F> &&r)
        {
            for(size_t i = 0; i < r.mElements.count(); i++)
            {
                addElement(r.mElements[i]);
            }
            return *this;
        }

        /// 積集合で結合します
        template<class I, class F>
        Set<T, H, E> &operator&=(const Set<T, I, F> &r)
        {
            leaveDuplicate(r);
            return *this;
        }

        /// 積集合で結合します
        template<class I, class F>
        Set<T, H, E> &operator&=(Set<T, I, F> &&r)
        {
            leaveDuplicate(r);
            return *this;
        }

        /// 差集合で削除します
        template<class I, class F>
        Set<T, H, E> &operator-=(const Set<T, I, F> &r)
        {
            for(size_t i = 0; i < r.mElements.count(); i++)
            {
                removeElement(r.mElements[i]);
            }
            return *this;
        }

        /// 差集合で削除します
        template<class I, class F>
        Set<T, H, E> &operator-=(Set<T, I, F> &&r)
        {
            for(size_t i = 0; i < r.mElements.count(); i++)
            {
                removeElement(r.mElements[i]);
            }
            return *this;
        }

        /// 含まれるか判定します
        bool operator()(const T &element) const
        {
            return NONE_INDEX_NUM != elementIndexOf(element, mElements, mIndexes);
        }

        /// 追加します
        Set<T, H, E> &add(const T &r)
        {
            operator|=(r);
            return *this;
        }

        /// 追加します
        Set<T, H, E> &add(T &&r)
        {
            operator|=(r);
            return *this;
        }

        /// 削除します
        Set<T, H, E> &remove(const T &r)
        {
            operator-=(r);
            return *this;
        }

        /// 削除します
        Set<T, H, E> &remove(T &&r)
        {
            operator-=(r);
            return *this;
        }

        /// 和集合で結合します
        template<class I, class F>
        Set<T, H, E> &add(const Set<T, I, F> &r)
        {
            return operator|=(r);
        }

        /// 和集合で結合します
        template<class I, class F>
        Set<T, H, E> &add(Set<T, I, F> &&r)
        {
            return operator|=(r);
        }

        /// 積集合で結合します
        template<class I, class F>
        Set<T, H, E> &intersect(const Set<T, I, F> &r)
        {
            return operator&=(r);
        }

        /// 積集合で結合します
        template<class I, class F>
        Set<T, H, E> &intersect(Set<T, I, F> &&r)
        {
            return operator&=(r);
        }

        /// 差集合で削除します
        template<class I, class F>
        Set<T, H, E> &remove(const Set<T, I, F> &r)
        {
            return operator-=(r);
        }

        /// 差集合で削除します
        template<class I, class F>
        Set<T, H, E> &remove(Set<T, I, F> &&r)
        {
            return operator-=(r);
        }

        /// 含まれるか判定します
        bool contains(const T &element) const
        {
            return NONE_INDEX_NUM != elementIndexOf(element, mElements, mIndexes);
        }

        /// 要素数を返します
        size_t count() const
        {
            return mElements.count();
        }

        /// クリアします
        void clear()
        {
            mElements.clear();
            mIndexes.clear();

            mElements.resize(INIT_ELEM_CNT_A);
            mIndexes.resize(INIT_ELEM_CNT_A);

            for(size_t i = 0; i < INIT_ELEM_CNT_A; i++)
            {
                mIndexes.add(NONE_INDEX_NUM);
            }
        }

        /// 先頭イテレータを返します
        ConstPointerItr<T> begin() const
        {
            return ConstPointerItr<T>(mElements);
        }

        /// 番兵イテレータを返します
        ConstPointerItr<T> end() const
        {
            return ConstPointerItr<T>(&mElements[mElements.count()]);
        }
    };

    /// 和集合です
    template<class T, class H, class I, class E, class F>
    Set<T, H, E> operator|(const Set<T, H, E> &l, Set<T, I, F> &r)
    {
        return Set<T, H, E>(l) |= r;
    }

    /// 積集合です
    template<class T, class H, class I, class E, class F>
    Set<T, H, E> operator&(const Set<T, H, E> &l, Set<T, I, F> &r)
    {
        return Set<T, H, E>(l) &= r;
    }

    /// 差集合です
    template<class T, class H, class I, class E, class F>
    Set<T, H, E> operator-(const Set<T, H, E> &l, Set<T, I, F> &r)
    {
        return Set<T, H, E>(l) -= r;
    }

}

#endif // !ELEKICORE_SET_HPP