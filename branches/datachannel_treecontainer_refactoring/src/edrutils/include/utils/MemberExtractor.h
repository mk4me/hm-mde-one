/********************************************************************
	created:  2016/01/29
	filename: MemberExtractor.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__MEMBEREXTRACTOR_H__
#define __HEADER_GUARD_UTILS__MEMBEREXTRACTOR_H__

#include <utils/pointer_traits.h>

namespace utils
{
	template<bool M>
	struct ExtractorBase
	{
		static const bool is_member_extractor = M;
	};

	//! TODO
	//template<typename E>
	//struct is_valid_member_extractor;

	//! Makro z nazw� extractora danego pola
#define MEMBER_EXTRACTOR_NAME(memberName) MemberExtractor##memberName

	//! Makro definuj�ce dany extractor generyczny
#define MEMBER_EXTRACTOR(memberName) \
struct MEMBER_EXTRACTOR_NAME(memberName) : public utils::ExtractorBase<true>{\
/* Wersja dla obiekt�w */ \
/* \tparam T Typ z kt�rego wypakowujemy membera */ \
/*	\param value Warto�� z kt�rej wyci�gamy membera */ \
/*	\param dummy Na potrzeby innych klas */ \
template<typename T> inline static auto extract(const T & value, ...) -> decltype((value.memberName)) { return value.memberName; }\
/* Wersja dla wska�nik�w */ \
/* \tparam T Typ z kt�rego wypakowujemy membera */ \
/*	\param value Warto�� z kt�rej wyci�gamy membera */ \
/*	\param dummy Na potrzeby innych klas */ \
template<typename T, ENABLE_IF(utils::is_general_pointer<T>::value)> inline static auto extract(const T & value, ...) -> decltype((value->memberName)) { return value->memberName; }\
};

	//! Struktura pomocnicza realizuj�ca prze�roczysto�� warto�ci
	struct TransparentExtractor : public ExtractorBase<false>
	{
		//! \tparam T Typ warto�ci
		template<typename T>
		//! \param value Warto��
		//! \param dummy Na potrzeby innych klas
		//! \return Warto��
		static inline const T & extract(const T & value, ...) { return value };
	};

	struct PointerExtractor : public ExtractorBase<false>
	{
		//! \tparam T Typ warto�ci
		template<typename T>
		//! \param value Warto��
		//! \param dummy Na potrzeby innych klas
		//! \return Warto��
		static inline auto extract(const T & value, ...) -> decltype(*value)
		{
			return *value;
		};
	};
}

#endif  // __HEADER_GUARD_UTILS__MEMBEREXTRACTOR_H__