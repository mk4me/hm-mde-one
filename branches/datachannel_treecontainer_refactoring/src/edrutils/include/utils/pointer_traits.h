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
	//! \tparam T Sprawdzany typ pod k�tem operatora ->
	template <typename T>
	//! Klasa pomocnicza przy wykrywaniu klas o charakterze smart pointer�w
	struct supports_pointer_operator_impl
	{
		//! \tparam U Badany typ
		template<typename U>
		//! \return Typ wy�uskiwany przez operator ->
		static auto test(U* p) -> decltype(p->operator->()); // checks function existence
		//! \tparam U Badany typ
		template<typename U>
		//! \return Fa�sz
		static auto test(...) -> std::false_type;
		//! Informacja czy typ ma charakter smart pointera
		using type = typename std::is_pointer<decltype(test<T>(0))>::type; // checks return type is some kind of floating point
	};
	//! \tparam T Sprawdzany typ pod k�tem operatora ->
	template <typename T>
	//! Klasa z informacj� o podobie�stwie do smart pointera
	struct supports_pointer_operator : public supports_pointer_operator_impl<T>::type {};

	//! \tparam T Typ badany pod k�tem bycia podobnym do smart poitner�w r�nej ma�ci
	template<typename T>
	//! Struktura nios�ca informacj� czy badany tym ma znamiona smart pointera
	struct is_like_smart_pointer : public std::integral_constant<bool, std::is_class<T>::value && supports_pointer_operator<T>::value> {};

	//! \tparam T Badany typ
	//! \tparam Specjalizacja dla zwyk�ych wksa�nik�w
	template<typename T, typename std::enable_if<std::is_pointer<T>::value || is_like_smart_pointer<T>::value>::type * = 0>
	struct pointed_type_helper
	{
		//! Typ wskazywany przez wska�nik
		using type = typename typename std::remove_pointer<typename std::conditional<std::is_pointer<T>::value, T, decltype(std::declval<const T>().operator->())>::type>::type;
	};	

	//! \tparam T Badany typ wska�nika
	template<typename T>
	struct pointed_type
	{
		//! Typ wskazywany przez wska�nik lub wrapowany pod wska�nikiem
		using type = typename remove_toplevel<typename pointed_type_helper<T>::type>::type;
	};
}

#endif  // __HEADER_GUARD_UTILS__POINTER_TRAITS_H__