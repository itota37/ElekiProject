/// @file component.hpp
/// @version 1.22.6
/// @copyright © 2022 Taichi Ito
/// ECSアーキテクチャのコンポーネントシステムを提供します

#ifndef ELEKICORE_COMPONENT_HPP
#define ELEKICORE_COMPONENT_HPP

#include "array.hpp"
#include "map.hpp"
#include "serialization.hpp"
#include "entity.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{


	/// コンポーネントメタ情報インタフェースです
	class IComponent
	{
	public:

		/// 型が等しいか比較します
		virtual bool operator==(const IComponent &r) const = 0;

		/// 型が等しくないか比較します
		virtual bool operator!=(const IComponent &r) const = 0;

		/// 型名を取得します
		virtual String name() const = 0;

		/// 型サイズを取得します
		virtual size_t size() const = 0;

		/// 初期化します
		virtual void initialize(void *instance) const = 0;

		/// 終了化します
		virtual void finalize(void *instance) const = 0;

		/// シリアライズします
		virtual void serialize(Serializer &serializer, const void *instance) const = 0;

		/// デシリアライズします
		virtual void deserialize(Deserializer &deserializer, void *instance) const = 0;

	};

	/// コンポーネントメタ情報クラスです
	template<class T>
	class Component
	{

		String mName;
		size_t mSize;

	public:

		/// コンストラクタ
		Component()
			: mName(typenameOf<T>())
			, mSize(sizeof(T))
		{}

	};


	/// 型の集合を表現するクラスです
	class Archetype
	{
		List<IComponent> mTypes;            // Type配列
		Map<String, size_t> mIndexes; // 型名からTypeの位置を取得するための連想配列

	public:

		/// 位置からコンポーネント情報にアクセスします
		const IComponent *operator[](size_t index) const;

		/// 型名からコンポーネント情報にアクセスします
		const IComponent *operator[](const String &name) const;

		/// コンポーネント名から位置を取得します
		size_t operator()(const String &name) const;

		/// コンポーネントの集合が等しいか比較します
		bool operator==(const Archetype &r) const;

		/// コンポーネントの集合が等しくないか比較します
		bool operator!=(const Archetype &r) const;

		/// 位置からコンポーネント情報にアクセスします
		const IComponent *atComponent(size_t index) const;

		/// 型名からコンポーネント情報にアクセスします
		const IComponent *atComponent(const String &name) const;

		/// コンポーネント名から位置を取得します
		size_t atIndex(const String &name) const;
	};


	/// コンポーネントとエンティティのセットを複数保持するチャンククラスです
	class Chunk: public ISerializable
	{

		Archetype mArchetype;
		size_t mMaxCount;
		size_t mUseCount;

		List<Entity *> mEntitys;
		List<u8 *> mBuffers;
	};


	/// 型の集合の論理積を表現するクラスです
	template<class...Ts>
	class And
	{

	};


	/// 型の集合の論理和を表現するクラスです
	template<class...Ts>
	class Or
	{

	};

	struct _ICompType
	{
		virtual bool operator()(const Archetype &archetype, size_t index) = 0;
	};


	template<class...Ts>
	struct _CompType {};

	template<class T, class...Ts>
	struct _CompType <T, Ts...>
	{
		bool operator()(const Archetype &archetype, size_t index) override
		{

		}
	};


	template<class...Ts, class...Us>
	struct _CompType<Or<Ts...>, Us...>: _ICompType
	{
		bool operator()(const Archetype &archetype, size_t index) override
		{

		}
	};

}

#endif // !ELEKICORE_COMPONENT_HPP