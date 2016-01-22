/********************************************************************
	created:  2016/01/20
	filename: pointer_traits.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__POINTER_TRAITS_H__
#define __HEADER_GUARD_UTILS__POINTER_TRAITS_H__

#include <type_traits>
#include <utils/Utils.h>
#include <utils/SmartPtr.h>

namespace utils
{
	//! \tparam T Sprawdzany typ pod k¹tem operatora ->
	template <typename T>
	//! Klasa pomocnicza przy wykrywaniu klas o charakterze smart pointerów
	struct supports_pointer_operator_impl
	{
		//! \tparam U Badany typ
		template<typename U>
		//! \return Typ wy³uskiwany przez operator ->
		static auto test(U* p) -> decltype(p->operator->()); // checks function existence
		//! \tparam U Badany typ
		template<typename U>
		//! \return Fa³sz
		static auto test(...) -> std::false_type;
		//! Informacja czy typ ma charakter smart pointera
		using type = typename std::is_pointer<decltype(test<T>(0))>::type; // checks return type is some kind of floating point
	};
	//! \tparam T Sprawdzany typ pod k¹tem operatora ->
	template <typename T>
	//! Klasa z informacj¹ o podobieñstwie do smart pointera
	struct supports_pointer_operator : public supports_pointer_operator_impl<T>::type {};

	//! \tparam T Typ badany pod k¹tem bycia podobnym do smart poitnerów ró¿nej maœci
	template<typename T>
	//! Struktura nios¹ca informacjê czy badany tym ma znamiona smart pointera
	struct is_like_smart_pointer : public std::integral_constant<bool, std::is_class<T>::value && supports_pointer_operator<T>::value> {};

	//! \tparam T Badany typ
	//! \tparam Specjalizacja dla zwyk³ych wksaŸników
	template<typename T, typename std::enable_if<std::is_pointer<T>::value || is_like_smart_pointer<T>::value>::type * = 0>
	struct pointed_type_helper
	{
		//! Typ wskazywany przez wskaŸnik
		using type = typename typename std::remove_pointer<typename std::conditional<std::is_pointer<T>::value, T, decltype(std::declval<const T>().operator->())>::type>::type;
	};	

	//! \tparam T Badany typ wskaŸnika
	template<typename T>
	struct pointed_type
	{
		//! Typ wskazywany przez wskaŸnik lub wrapowany pod wskaŸnikiem
		using type = typename remove_toplevel<typename pointed_type_helper<T>::type>::type;
	};
}

#endif  // __HEADER_GUARD_UTILS__POINTER_TRAITS_H__