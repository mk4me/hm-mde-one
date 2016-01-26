/********************************************************************
	created:  2016/01/20
	filename: array_traits.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__ARRAY_TRAITS_H__
#define __HEADER_GUARD_UTILS__ARRAY_TRAITS_H__

#include <type_traits>
#include <utils/Utils.h>

namespace utils
{
	namespace impl
	{
		//! \tparam A Typ tablicowy, kt�ry weryfikujemy
		template<typename A>
		//! Struktura pomocnicza przy weryfikacji tablic (operator [])
		struct array_check
		{
			//! Weryfikacja operatora [] w wersji const
			template<typename U> static auto check(const U * u) -> decltype(u->operator[](0));
			//! Fallback
			template<typename U> static void check(...);

			using type = typename std::integral_constant<bool, !std::is_void<decltype(check<A>(0))>::value>::type;
		};
	}

	//! \tparam A Typ tablicowy, kt�ry weryfikujemy
	template<typename A>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct array_check : public impl::array_check<A>::type {};

	//! \tparam A Typ tablicy ajki weryfikujemy
	template<typename A>
	//! W�a�ciwy trait przepuszczaj�cy typy z mo�liwo�ci� indeksowania operatorem []
	struct is_general_array : public std::integral_constant<bool, std::is_array<A>::value || array_check<A>::value> {};

	namespace impl
	{
		//! \tparam T Typ z operatorem[]
		template<typename T>
		//! Klasa pomocnicza przy wyci�ganiu typu zwracanego przez operator[]
		struct array_type_helper
		{
			using type = decltype(std::declval<T>()[0]);
		};

		//! \tparam Typ przechowywany w tablicy
		//! \tparam N Rozmiar tablicy
		template<typename T, std::size_t N>
		//! Klasa pomocnicza przy wyci�ganiu typu zwracanego przez operator[]
		struct array_type_helper<T[N]>
		{
			using type = T;
		};
	}

	//! \tparam T Typ tablico-podobny dla kt�ego wyci�gamy typ zwracany przez operator[]
	//! \tparam dummy
	template<typename T, typename std::enable_if<is_general_array<T>::value>::type * = 0>
	//! Klasa pomocnicza przy wyci�ganiu typu zwracanego przez operator[]
	struct array_type : public impl::array_type_helper<T> {};

	namespace impl
	{
		//! \tparam T Typ tablicy
		//! \tparam dummy
		template<typename T, bool = false>
		//! Specjalizacja dla typ�w nie maj�cych charakteru tablic
		struct array_dimensions_helper
		{
			static const std::size_t dimensions = 0;
		};

		//! \tparam T Typ tablicy		
		template<typename T>
		//! Specjalizacja dla typ�w o charakterze tablicy
		struct array_dimensions_helper<T, true>
		{
			using _type = typename array_type<T>::type;
			static const std::size_t dimensions = 1 + array_dimensions_helper<_type, is_general_array<_type>::value>::dimensions;
		};
	}

	//! \tparam Typ badany pod k�tem bycia tablic�
	template<typename T>
	//! Wersja specjalizowana dla tablic - wyznacza faktyczny rozmiar tablicy
	struct array_dimensions : public impl::array_dimensions_helper<T, is_general_array<T>::value> {};

	namespace impl
	{
		template<typename T, std::size_t Idx>
		struct array_dimension_type_helper
		{
			using type = typename array_dimension_type_helper<typename array_type<T>::type, Idx - 1>::type;
		};

		template<typename T>
		struct array_dimension_type_helper<T, 0>
		{
			using type = T;
		};
	}

	//! \tparam T Typ tablicy
	//! \tparam Dim Numer wymiaru tablicy dla kt�ego chcemy pobra� typ
	//! \tparam dummy Warunek czy typ faktycznie ma charakter tablicy
	template<typename T, std::size_t Dim, typename std::enable_if<is_general_array<T>::value>::type * = 0>
	struct array_dimension_type
	{
		static_assert(Dim > 0 && Dim <= array_dimensions<T>::dimensions, "Index zagnie�d�enia musi by� w przedziale <1 - ilo�� wymiar�w tablicy>");

		using type = typename impl::array_dimension_type_helper<typename array_type<T>::type, Dim - 1>::type;
		using clean_type = typename remove_toplevel<type>::type;

	};

	//! \tparam T Typ tablicy dla kt�rej wyci�gamy typ ostatniego wymiaru		
	template<typename T>
	//! Klasa pomocnicza przy wycaganiu typu ostatniego wymiaru
	struct array_final_dimension_type : public array_dimension_type<T, array_dimensions<T>::dimensions> {};

	//! \tparam T Typ tablicy
	template <typename T, typename std::enable_if<is_general_array<T>::value>::type * = 0>
	//! \param array Tablica
	//! \param ... Na potrzeby innych klas
	//! \return Wypakowany element
	static inline auto extract(const T & array, const std::size_t idx) -> decltype(array[idx])
	{
		return array[idx];
	}	

	//! Wypakowuje z tablicy zadanyc element
	//! \tparam Element Indeks elementu w tablicy	
	template <std::size_t Element>
	struct StaticArrayElementExtractor
	{
		static_assert(Element >= 0, "Element index must be greater or equal 0");

		//! \tparam T Typ tablicy
		template <typename T>
		//! \param array Tablica
		//! \param ... Na potrzeby innych klas
		//! \return Wypakowany element
		static inline auto extract(const T & array, ...) -> decltype(extract(array, Element))
		{
			return extract(array, Element);
		}		
	};

	//! Klasa pozwalaj�ca wypakowywa� dane z wektor�w
	class ArrayElementExtractor
	{
	public:
		//! \param idx Index obiketu kt�ry chcemy wypakowywa� z wektora
		ArrayElementExtractor(const std::size_t idx) : idx(idx) {}
		//! \param Other Inny kopiowany extractor
		ArrayElementExtractor(const ArrayElementExtractor & Other) : idx(Other.idx) {}
		//! Destruktor
		~ArrayElementExtractor() {}

		//! \tparam T Typ tablicy
		template <typename T>
		//! \param array Tablica
		//! \param idx Indeks elementu kt�ry chcemy wyci�gn�c
		//! \return Wypakowany element
		static inline auto extract(const T & array, const std::size_t idx) -> decltype(::extract(array, idx))
		{
			return ::extract(array, idx);
		}

		//! \tparam T Typ tablicy
		template <typename T>
		//! \param array Tablica		
		//! \return Wypakowany element
		inline auto extract(const T & array) -> decltype(extract(array, idx))
		{
			return extract(array, idx);
		}

	private:
		//! Indeks spod kt�rego wybieramy dane z wektora
		const std::size_t idx;
	};

	//! Zwraca d�ugo�� tablicy
	//! \param fixedArray Tablica
	template <class T, std::size_t Size>
	//! \param fixedArray Tablica o znanej d�ugo�ci
	//! \return D�ugo�� tablicy
	static inline std::size_t size(const T(&fixedArray)[Size])
	{
		return Size;
	}

	struct ArraySizeExtractor
	{
		template<typename T>
		static inline std::size_t size(const T & array)
		{
			return ::size(array);
		}
	};

	//! \tparam T Typ elementu tablicy
	//! \tparam N Rozmiar tablicy
	template<typename T, std::size_t Size>
	//! \param fixedArray Tablica o znanej d�ugo�ci
	//! \return Czy tablica jest pusta
	static inline bool empty(const T(&fixedArray)[Size])
	{
		return false;
	}

	//! \tparam T Typ dla jakiego sprawdzamy czy jest pusty
	template<typename T>
	//! \param array Typ tablicy
	//! \return Czy kontener/tablica jest pusta
	static inline bool empty(const T & array)
	{
		return length(array) == 0;
	}

	struct ArrayEmptinessExtractor
	{
		template<typename T>
		static inline std::size_t empty(const T & array)
		{
			return ::empty(array);
		}
	};

	//! Zwraca rozmiar tablicy w bajtach
	//! \param fixedArray Tablica
	//! \return Rozmiar tablicy
	template <class T, std::size_t Size>
	static inline std::size_t memory_size(const T(&array)[Size])
	{
		return sizeof(array);
	}
}

#endif  // __HEADER_GUARD_UTILS__ARRAY_TRAITS_H__