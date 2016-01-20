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

namespace utils
{
	//! \tparam T Sprawdzany typ pod k�tem operatora ->
	template<typename T, typename = decltype(&T::operator->)>
	//! \param dummy
	//! \return Prawda je�li poprawnie dopasowany
	static std::true_type  supports_pointer_operator_test(const T&);
	//! \return Fa�sz je�li poprawnie dopasowany
	static std::false_type supports_pointer_operator_test(...);
	//! \tparam T Sprawdzany typ pod k�tem operatora ->
	template<typename T>
	//! Struktura b�d�ca pochodn� prawdy je�li typ wspiera operator->
	struct supports_pointer_operator : public decltype(supports_pointer_operator_test(std::declval<T>())) {};

	//! \tparam T Typ badany pod k�tem bycia podobnym do smart poitner�w r�nej ma�ci
	template<typename T>
	//! Struktura nios�ca informacj� czy badany tym ma znamiona smart pointera
	struct is_like_smart_pointer : public std::conditional<std::is_class<T>::value && supports_pointer_operator<T>::value, std::true_type, std::false_type>::type {};

	//! \tparam T Badany typ
	//! \tparam Specjalizacja dla zwyk�ych wksa�nik�w
	template<typename T, typename std::enable_if<std::is_pointer<T>::value || is_like_smart_pointer<T>::value>::type * = 0>
	struct pointed_type_helper
	{
		//! Typ wskazywany przez wska�nik
		using type = typename std::conditional<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, decltype(std::declval<const T>().operator->())>::type;
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