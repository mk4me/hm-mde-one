/********************************************************************
	created:  2016/01/21
	filename: Traits.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__TRAITS_H__
#define __HEADER_GUARD_DATAACCESSOR__TRAITS_H__

#include <datachannellib/Accessors.h>
#include <utils/pointer_traits.h>

namespace dataaccessor
{
	//! \tparam T Typ z kt�rego pr�bujemy wnioskowa� wska�nik
	template<typename T>
	//! Typ dyskretnego wska�nika na bazie akcesora
	using ExtractedDiscreteConstPtr = DiscreteAccessorConstPtr<typename T::value_type, typename T::argument_type>;

	//! \tparam T Typ z kt�rego pr�bujemy wnioskowa� wska�nik
	template<typename T>
	//! Typ funkcyjnego wska�nika na bazie akcesora
	using ExtractedFunctionConstPtr = FunctionAccessorConstPtr<typename T::value_type, typename T::argument_type>;

	namespace impl
	{
		//! \tparam T Typ badany pod k�tem opisu typ�w warto�ci i argument�w
		template <typename T>
		//! Klasa pomocnicza przy detekcji kompletnego opisu akcesora (typy warto�ci i argument�w)
		struct has_data_description
		{
			//! \tparam U Typ badany pod k�tem opisu argument�w
			template<typename U>
			//! \return Prawda je�li uda si� dopasowa�
			static auto test_argument_type(typename U::argument_type* p)->std::true_type;
			//! \tparam U Typ badany pod k�tem opisu argument�w
			template<typename U>
			//! \return Fa�sz je�li uda si� dopasowa�
			static auto test_argument_type(...)->std::false_type;

			//! \tparam U Typ badany pod k�tem opisu warto�ci
			template<typename U>
			//! \return Prawda je�li uda si� dopasowa�
			static auto test_value_type(typename U::value_type* p)->std::true_type;
			//! \tparam U Typ badany pod k�tem opisu warto�ci
			template<typename U>
			//! \return Fa�sz je�li uda si� dopasowa�
			static auto test_value_type(...)->std::false_type;

			//! Typ opisuj�cy dost�pno�� informacji o typach warto�ci i argument�w
			using type = std::integral_constant<bool, std::is_same<decltype(test_argument_type<T>(0)), std::true_type>::value &&
				std::is_same<decltype(test_value_type<T>(0)), std::true_type>::value>;
		};
	}

	//! \tparam T Typ badany pod k�tem opisu typ�w warto�ci i argument�w
	template<typename T>
	//! Klasa z informacj� o kompletnym opisie akcesora
	struct has_data_description : public impl::has_data_description<T>::type {};	

	//! \tparam T Typ z kt�rego wnioskujemy dyskretny akcesor
	template<typename T>
	//! Typ dyskretnego akcesora na bazie innego typu
	using ExtractedDiscreteAccessor = IDiscreteAccessorT<typename T::value_type, typename T::argument_type>;

	//! \tparam T Typ z kt�rego wnioskujemy ci�g�y akcesor
	template<typename T>
	//! Typ ci�g�ego akcesora na bazie innego typu
	using ExtractedFunctionAccessor = IFunctionAccessorT<typename T::value_type, typename T::argument_type>;

	namespace impl
	{
		//! \tparam T Badany typ pod k�tem dyskretnego akcesora
		//! \tparam dummy
		template <typename T, bool>
		//! Domy�lna implementacja - fa�sz
		struct is_valid_discrete_accessor : public std::false_type {};

		//! \tparam T Badany typ pod k�tem dyskretnego akcesora
		template <typename T>
		//! Implementacja kiedy typ ma opis odpowiadaj�cy akcesorowi - prawda kiedy spe�nione s� pozosta�e warunki
		struct is_valid_discrete_accessor<T, true> : public std::integral_constant<bool, std::is_base_of<ExtractedDiscreteAccessor<T>, T>::value && !std::is_base_of<ExtractedFunctionAccessor<T>, T>::value> {};
	}

	//! \tparam T Badany typ pod k�tem dyskretnego akcesora
	template<typename T>
	//! Klasa z informacj� czy badany tym ma charakter dyskretnego akcesora
	struct is_valid_discrete_accessor : public impl::is_valid_discrete_accessor<T, has_data_description<T>::value>::type {};

	namespace impl
	{
		//! \tparam T Badany typ pod k�tem funkcyjnego akcesora
		//! \tparam dummy
		template <typename T, bool>
		//! Domy�lna implementacja - fa�sz
		struct is_valid_function_accessor : public std::false_type {};

		//! \tparam T Badany typ pod k�tem funkcyjnego akcesora
		template <typename T>
		//! Implementacja kiedy typ ma opis odpowiadaj�cy akcesorowi - prawda kiedy spe�nione s� pozosta�e warunki
		struct is_valid_function_accessor<T, true> : public std::integral_constant<bool, !std::is_base_of<ExtractedDiscreteAccessor<T>, T>::value && std::is_base_of<ExtractedFunctionAccessor<T>, T>::value> {};
	}

	//! \tparam T Badany typ pod k�tem funkcyjnego akcesora
	template<typename T>
	//! Klasa z informacj� czy badany tym ma charakter funkcyjnego akcesora
	struct is_valid_function_accessor : public impl::is_valid_function_accessor<T, has_data_description<T>::value>::type {};	

	//! \tparam T Typ sprawdzany pod k�tem poprawno�ci akcesora
	template<typename T>
	//! Typ z informacj� o poprawno�ci akcesora (jeden z interfejs�w)
	struct is_valid_accessor : public utils::xor_check<is_valid_function_accessor<T>::value, is_valid_discrete_accessor<T>::value>::type {};

	//! \tparam T Typ implementuj�cy jeden z interfejs�w akcesor�w (dyskretny/funkcja)
	//! \tparam dummy Weryfikacja �e mamy do czynienia z poprawnym akcesorem
	template<typename T, ENABLE_IF(is_valid_accessor<T>::value)>
	struct accessor_interface
	{
		using type = typename std::conditional<is_valid_discrete_accessor<T>::value, typename ExtractedDiscreteAccessor<T>::type, typename ExtractedFunctionAccessor<T>::type>::type;
	};

	namespace impl
	{
		//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
		//! \tparam dummy
		template<typename T, bool>
		//! Cze�ciowa specjalizacja - fa�sz
		struct is_valid_discrete_accessor_ptr : public std::false_type {};

		//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
		template<typename T>
		//! Cz�ciowa specjalizacja - weryfikacja warunk�w	
		struct is_valid_discrete_accessor_ptr<T, true> : public std::is_convertible<T, ExtractedDiscreteConstPtr<typename utils::clean_pointed_type<T>::type>>::type {};
	}

	//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
	template<typename T>
	//! Cz�ciowa specjalizacja - weryfikacja warunk�w
	struct is_valid_discrete_accessor_ptr : public impl::is_valid_discrete_accessor_ptr<T, utils::is_like_smart_pointer<T>::value> {};

	namespace impl
	{
		//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
		//! \tparam dummy
		template<typename T, bool>
		//! Cze�ciowa specjalizacja - fa�sz
		struct is_valid_function_accessor_ptr : public std::false_type {};

		//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
		template<typename T>
		//! Cz�ciowa specjalizacja - weryfikacja warunk�w	
		struct is_valid_function_accessor_ptr<T, true> : public std::is_convertible<T, ExtractedFunctionConstPtr<typename utils::clean_pointed_type<T>::type>>::type {};
	}

	//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
	template<typename T>
	//! Cz�ciowa specjalizacja - weryfikacja warunk�w
	struct is_valid_function_accessor_ptr : public impl::is_valid_function_accessor_ptr<T, utils::is_like_smart_pointer<T>::value> {};

	//! \tparam T Typ weryfikowany pod k�tem poprawno�ci wska�nika do akcesora
	//! \tparam dummy
	template<typename T>
	//! Cze�ciowa specjalizacja - fa�sz
	struct is_valid_accessor_ptr : public utils::xor_check<is_valid_function_accessor_ptr<T>::value, is_valid_discrete_accessor_ptr<T>::value>::type {};	
	//! \tparam T Typ wskaxnika do akcesora
	//! \tparam dummy Weryfikacja wks��nika
	template<typename T, ENABLE_IF(is_valid_accessor_ptr<T>::value ||
		(std::is_pointer<T>::value && is_valid_accessor<typename utils::clean_pointed_type<T>::type>::value))>
	//! Klasa pomocnicza do wyci�gania informacji o akcesorze ze wska�nik�w
	struct AccessorPointerDesc
	{
		using accessor = typename utils::clean_pointed_type<T>::type;
		using value_type = typename accessor::value_type;
		using argument_type = typename accessor::argument_type;
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__TRAITS_H__