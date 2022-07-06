#include "elekicore/entity.hpp"

using namespace ElekiEngine;

// コンストラクタ
ElekiEngine::Entity::Entity(Entity **point)
	: mPoint(point)
	, mPrev(this)
	, mNext(this)
{}

// 接続を切ります
void ElekiEngine::Entity::disconnect(Entity * *oldEntity)
{
	if((*oldEntity) == this)
	{
		(*oldEntity) = mNext;
	}
	mPrev->mNext = mNext;
	mNext->mPrev = mPrev;
}

// 接続します
void ElekiEngine::Entity::connect(Entity **newEntity, Entity **oldEntity)
{
	if(oldEntity)
	{
		disconnect(oldEntity);
	}

	auto newPoint = *newEntity;

	mPrev = newPoint->mPrev;
	mNext = newPoint;
	newPoint->mPrev->mNext = this;
	newPoint->mPrev = this;
}

// コピーコンストラクタ
ElekiEngine::Entity::Entity(const Entity &entity)
	: mPoint(entity.mPoint)
{
	connect(mPoint, nullptr);
}

// ムーブコンストラクタ
ElekiEngine::Entity::Entity(Entity &&entity) noexcept
	: mPoint(entity.mPoint)
{
	connect(mPoint, nullptr);
}

// デストラクタ
ElekiEngine::Entity::~Entity()
{
	disconnect(mPoint);

	if(mNext == this)
	{
		// エンティティが無効になる
	}
}

// コピー代入演算子
Entity &ElekiEngine::Entity::operator=(const Entity & entity)
{
	connect(entity.mPoint, mPoint);
	return *this;
}

// ムーブ代入演算子
Entity &ElekiEngine::Entity::operator=(Entity &&entity) noexcept
{
	connect(entity.mPoint, mPoint);
	return *this;
}

// エンティティが等しいか判定します
bool ElekiEngine::Entity::operator==(const Entity &r) const
{
	return mPoint == r.mPoint;
}

// エンティティが等しくないか判定します
bool ElekiEngine::Entity::operator!=(const Entity &r) const
{
	return mPoint != r.mPoint;
}

// エンティティが超過か判定します
bool ElekiEngine::Entity::operator>(const Entity &r) const
{
	return mPoint > r.mPoint;
}

// エンティティが未満か判定します
bool ElekiEngine::Entity::operator<(const Entity &r) const
{
	return mPoint < r.mPoint;
}

// エンティティが以上か判定します
bool ElekiEngine::Entity::operator>=(const Entity &r) const
{
	return mPoint >= r.mPoint;
}

// エンティティが以下か判定します
bool ElekiEngine::Entity::operator<=(const Entity &r) const
{
	return mPoint <= r.mPoint;
}

// エンティティが有効か判定します
ElekiEngine::Entity::operator bool() const
{
	return mPoint != nullptr;
}