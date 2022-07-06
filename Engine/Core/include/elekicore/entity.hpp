/// @file entity.hpp
/// @version 1.22.6
/// @copyright © 2022 Taichi Ito
/// ECSアーキテクチャのエンティティシステムを提供します

#ifndef ELEKICORE_ENTITY_HPP
#define ELEKICORE_ENTITY_HPP

#include "integer.hpp"
#include "array.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{

    /// コンポーネントとエンティティのセットを複数保持するチャンククラスです
    class Chunk;

    /// エンティティクラスです
    class Entity
    {

        friend class Chunk;

        Entity **mPoint; // このエンティティを表すチャンク内の位置
        Entity *mPrev;   // 前の変数への参照
        Entity *mNext;   // 次の変数への参照

        // コンストラクタ
        Entity(Entity **point);

        // 接続を切ります
        void disconnect(Entity **oldEntity);

        // 接続します
        void connect(Entity **newEntity, Entity **oldEntity);

    public:

        /// コピーコンストラクタ
        Entity(const Entity &entity);

        /// ムーブコンストラクタ
        Entity(Entity &&entity) noexcept;

        /// デストラクタ
        ~Entity();

        /// コピー代入演算子
        Entity &operator=(const Entity &entity);

        /// ムーブ代入演算子
        Entity &operator=(Entity &&entity) noexcept;

        /// エンティティが等しいか判定します
        bool operator==(const Entity &r) const;

        /// エンティティが等しくないか判定します
        bool operator!=(const Entity &r) const;

        /// エンティティが超過か判定します
        bool operator>(const Entity &r) const;

        /// エンティティが未満か判定します
        bool operator<(const Entity &r) const;

        /// エンティティが以上か判定します
        bool operator>=(const Entity &r) const;

        /// エンティティが以下か判定します
        bool operator<=(const Entity &r) const;

        /// エンティティが有効か判定します
        operator bool() const;

    };
}

#endif // !ELEKICORE_ENTITY_HPP