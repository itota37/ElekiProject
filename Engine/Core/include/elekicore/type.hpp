/// @file type.hpp
/// @version 1.22.5
/// @copyright © 2022 Taichi Ito
/// 動的型情報と動的インスタンスを提供します

#ifndef ELEKICORE_TYPE_HPP
#define ELEKICORE_TYPE_HPP

#include "pointer.hpp"
#include "string.hpp"
#include "array.hpp"
#include "map.hpp"
#include "utility.hpp"
#include "serialization.hpp"

/// ELEKi ENGINE
namespace ElekiEngine
{
	/// 型のメタインタフェースです
	class IMeta
	{
	public:

		/// 型名を返します
		virtual String name() const = 0;

		/// 型サイズを返します
		virtual size_t size() const = 0;

		/// 初期化します
		virtual void construct(void *instance) const = 0;

		/// 終了化します
		virtual void destruct(void *instance) const = 0;

	};


	/// 型のメタ情報クラスです
	template<class T>
	class Meta: public IMeta
	{

		String mName; // 型名
		size_t mSize; // 型サイズ

	protected:

		/// コンストラクタ
		Meta()
			: mName(typenameOf<T>())
			, mSize(sizeof(T))
		{}

	public:

		/// 型名を返します
		String name() const override
		{
			return mName;
		}

		/// 型サイズを返します
		size_t size() const override
		{
			return mSize;
		}

		/// 初期化します
		virtual void construct(void *instance) const override
		{}

		/// 終了化します
		virtual void destruct(void *instance) const override
		{}

	};


	/// 動的型情報クラスです
	class ELEKICORE_EXPORT Type
	{

		friend Type typeByName(const String &name);

		IMeta *mMeta; // メタ情報の参照

		// コンストラクタ
		Type(IMeta *meta);

	public:


		/// 型が等しいか比較します
		bool operator==(const Type &r) const;

		/// 型が等しくないか比較します
		bool operator!=(const Type &r) const;

		/// 型名を返します
		String name() const;

		/// 型サイズを返します
		size_t size() const;

		/// 初期化します
		void construct(void *instance) const;

		/// 終了化します
		void destruct(void *instance) const;


	};

	/// 型名から動的型情報を取得します
	Type ELEKICORE_EXPORT typeByName(const String &name);

	/// 動的型情報を取得します
	template<class T>
	Type typeOf()
	{
		return typeByName(typenameOf<T>())
	}

	/// 動的型情報を取得します
	template<class T>
	Type typeOf(const T &value)
	{
		return typeByName(typenameOf(value))
	}


}

#endif // !ELEKICORE_TYPE_HPP