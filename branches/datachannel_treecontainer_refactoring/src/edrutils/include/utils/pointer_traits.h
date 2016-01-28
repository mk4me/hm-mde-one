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
	namespace impl {
		//! \tparam T Sprawdzany typ pod k�tem operatora ->
		template <typename T>
		//! Klasa pomocnicza przy wykrywaniu klas o charakterze smart pointer�w
		struct supports_pointer_operator
		{
			//! \tparam U Badany typ
			template<typename U>
			//! \return Typ wy�uskiwany przez operator ->
			static auto test(U* p) -> decltype(p->operator->()); // checks function existence
			//! \tparam U Badany typ
			template<typename U>
			//! \return Fa�sz
			static auto test(...)->std::false_type;
			//! Informacja czy typ ma charakter smart pointera
			using type = typename std::is_pointer<decltype(test<T>(0))>::type; // checks return type is some kind of floating point
		};
	}
	//! \tparam T Sprawdzany typ pod k�tem operatora ->
	template <typename T>
	//! Klasa z informacj� o podobie�stwie do smart pointera
	struct supports_pointer_operator : public impl::supports_pointer_operator<T>::type {};

	//! \tparam T Typ badany pod k�tem bycia podobnym do smart poitner�w r�nej ma�ci
	template<typename T>
	//! Struktura nios�ca informacj� czy badany tym ma znamiona smart pointera
	struct is_like_smart_pointer : public std::integral_constant<bool, std::is_class<T>::value && supports_pointer_operator<T>::value> {};

	//! \tparam T Typ badany pod k�tem bycia podobnym do smart poitner�w r�nej ma�ci
	template<typename T>
	//! Struktura nios�ca informacj� czy badany tym ma znamiona smart pointera
	struct is_general_pointer : public std::integral_constant<bool, std::is_pointer<T>::value || is_like_smart_pointer<T>::value> {};

	namespace impl
	{
		//! \tparam T Badany typ
		//! \tparam Specjalizacja dla zwyk�ych wksa�nik�w
		template<typename T, typename std::enable_if<is_general_pointer<T>::value>::type * = 0>
		//! Klasa pomocnicza przy wyci�ganiu typ�w wskazywanych przez wskaxniki  i intelig�tne wska�niki
		struct pointed_type_helper
		{
			//! Typ wskazywany przez wska�nik
			using type = typename typename std::remove_pointer<typename std::conditional<std::is_pointer<T>::value, T, decltype(std::declval<const T>().operator->())>::type>::type;
		};
	}

	//! \tparam T Badany typ wska�nika
	template<typename T>
	//! Klasa pomocnicza przy wyci�ganiu ca�ych typ�w wksazywanych przez wska�niki
	struct pointed_type
	{
		//! Typ wskazywany przez wska�nik lub wrapowany pod wska�nikiem
		using type = typename impl::pointed_type_helper<T>::type;
	};

	//! \tparam T Badany typ wska�nika
	template<typename T>
	//! Klasa pomocnicza przy wyci�ganiu "czystych" typ�w wksazywanych przez wska�niki
	struct clean_pointed_type
	{
		//! Typ wskazywany przez wska�nik lub wrapowany pod wska�nikiem
		using type = typename remove_toplevel<typename impl::pointed_type_helper<T>::type>::type;
	};

	namespace impl
	{
		//! \tparam T Typ wska�nika
		//! \tparam dummy
		template<typename T, bool = false>
		//! Specjalizacja dla typ�w nie maj�cych charakteru wska�nika
		struct pointer_levels_helper
		{
			static const std::size_t levels = 0;
		};

		//! \tparam T Typ tablicy		
		template<typename T>
		//! Specjalizacja dla typ�w o charakterze tablicy
		struct pointer_levels_helper<T, true>
		{
			using _type = typename pointed_type<T>::type;
			static const std::size_t levels = 1 + pointer_levels_helper<_type, is_general_pointer<_type>::value>::levels;
		};
	}

	//! \tparam Typ badany pod k�tem bycia tablic�
	template<typename T>
	//! Wersja specjalizowana dla tablic - wyznacza faktyczny rozmiar tablicy
	struct pointer_levels : public impl::pointer_levels_helper<T, is_general_pointer<T>::value> {};

	namespace impl
	{
		template<typename T, std::size_t Idx>
		struct pointer_level_type_helper
		{
			using type = typename pointer_level_type_helper<typename pointed_type<T>::type, Idx - 1>::type;
		};

		template<typename T>
		struct pointer_level_type_helper<T, 0>
		{
			using type = T;
		};
	}

	//! \tparam T Typ tablicy
	//! \tparam Dim Numer wymiaru tablicy dla kt�ego chcemy pobra� typ
	//! \tparam dummy Warunek czy typ faktycznie ma charakter tablicy
	template<typename T, std::size_t Level, typename std::enable_if<is_general_pointer<T>::value>::type * = 0>
	struct pointer_level_type
	{
		static_assert(Level > 0 && Level <= pointer_levels<T>::levels, "Index zagnie�d�enia musi by� w przedziale <1 - ilo�� wymiar�w tablicy>");

		using type = typename impl::pointer_level_type_helper<typename pointed_type<T>::type, Level - 1>::type;
		using clean_type = typename remove_toplevel<type>::type;

	};

	//! \tparam T Typ tablicy dla kt�rej wyci�gamy typ ostatniego wymiaru		
	template<typename T>
	//! Klasa pomocnicza przy wycaganiu typu ostatniego wymiaru
	struct pointer_final_level_type : public pointer_level_type<T, pointer_levels<T>::levels> {};
}

#endif  // __HEADER_GUARD_UTILS__POINTER_TRAITS_H__