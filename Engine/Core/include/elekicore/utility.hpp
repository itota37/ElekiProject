/// @file utility.hpp
/// @version 1.22.6
/// @copyright © 2022 Taichi Ito
/// 有用な機能を提供します

#ifndef ELEKICORE_UTILITY_HPP
#define ELEKICORE_UTILITY_HPP

#if ELEKI_COMPILER_GCC || ELEKI_COMPILER_CLANG
#include <cxxabi.h>
#endif

#include "string.hpp"


/// ELEKi ENGINE
namespace ElekiEngine
{

	/// 型名を取得します
	inline String typenameByTypeinfo(const std::type_info &info)
	{
	#if ELEKI_COMPILER_VC


		// VisualC++
		String name = info.name();
		// 型名より前を省く
		switch(name[0])
		{
			case 'c': name = name.sub(6); break;                                  // class
			case 's': name = name.sub(7); break;                                  // struct
			case 'u': name = name.sub(6); break;                                  // union
			case 'e':                                                             // enum
			{
				if(11 < name.count() && ' ' == name.count()) name = name.sub(11); // enum class 
				else name = name.sub(5);                                          // enum
				break;
			}
		}
		// 型名より後ろを省く
		size_t index = name.indexOf(' ');
		return (index < name.count() ? name.sub(index, true) : name);


	#elif ELEKI_COMPILER_GCC || ELEKI_COMPILER_CLANG


		// GCC or Clang
		int status = 0;
		cName = abi::__cxa_demangle(info.name(), 0, 0, &status);
		String name = cName;
		free(cName);
		return name;


	#endif
	}

	/// 型名を取得します
	template<class T>
	String typenameOf(const T &value)
	{
		return typenameByTypeinfo(typeid(value));
	}

	/// 型名を取得します
	template<class T>
	String typenameOf()
	{
		return typenameByTypeinfo(typeid(T));
	}


	/// バイトオーダーの種類の列挙です
	enum class EEndian: u8
	{

		MIDDLE = 0, ///< BIGまたはLITTLEでない方法です
		BIG = 1,    ///< byte列に対して桁を降順に挿入する方法です
		LITTLE = 2, ///< byte列に対して桁を昇順に挿入する方法です

	};


	/// このマシンのバイトオーダーを取得します
	inline EEndian endian()
	{

		u32 value = 0x00010203;
		u8 *byteValue = (u8 *) (void *) &value;

		switch(byteValue[0])
		{
			case 0:  return EEndian::BIG;
			case 3:  return EEndian::LITTLE;
			default: return EEndian::MIDDLE;
		}

	}

}

#endif // !ELEKICORE_UTILITY_HPP