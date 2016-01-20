/********************************************************************
	created:  2016/01/20
	filename: array_traits.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__ARRAY_TRAITS_H__
#define __HEADER_GUARD_UTILS__ARRAY_TRAITS_H__

#include <type_traits>

namespace utils
{
	// ------------------------------------------------------------------------------
	//! Informacje o tablicach
	struct ArrayTraits
	{
		//! Wypakowuje z tablicy zadanyc element
		//! \tparam Element Indeks elementu w tablicy	
		template <std::size_t Element>
		struct StaticElementExtractor
		{
			static_assert(Element >= 0, "Element index must be greater or equal 0");

			//! \tparam T Typ tablicy
			template <typename T>
			//! \param array Tablica
			//! \return Wypakowany element
			static inline auto extract(const T & array) -> decltype((array[Element]))
			{
				return array[Element];
			}

			//! \tparam T Typ elementu tablicy
			//! \tparam Size Rozmiar tablicy
			template <typename T, std::size_t Size>
			//! \param array Tablica
			//! \return Wypakowany element
			static inline const T & extract(const T(&array)[Size])
			{
				static_assert(Element < Size, "Element index out of range");
				return array[Element];
			}
		};

		//! Klasa pozwalaj¹ca wypakowywaæ dane z wektorów
		class ElementExtractor
		{
		public:
			//! \param idx Index obiketu który chcemy wypakowywaæ z wektora
			ElementExtractor(const std::size_t idx) : idx(idx) {}
			//! \param Other Inny kopiowany extractor
			ElementExtractor(const ElementExtractor & Other) : idx(Other.idx) {}
			//! Destruktor
			~ElementExtractor() {}

			//! \tparam T Typ tablicy
			template <typename T>
			//! \param array Tablica
			//! \return Wypakowany element
			inline auto extract(const T & array) -> decltype((array[idx]))
			{
				return array[idx];
			}

			//! \tparam T Typ elementu tablicy
			//! \tparam Size Rozmiar tablicy
			template <typename T, std::size_t Size>
			//! \param array Tablica
			//! \return Wypakowany element
			inline const T & extract(const T(&array)[Size])
			{
				return array[idx];
			}

		private:
			//! Indeks spod którego wybieramy dane z wektora
			const std::size_t idx;
		};

		//! Zwraca d³ugoœæ tablicy
		//! \param fixedArray Tablica
		//! \return D³ugoœæ tablicy
		template <class T>
		static inline std::size_t length(const T & array)
		{
			return array.size();
		}

		//! Zwraca d³ugoœæ tablicy
		//! \param fixedArray Tablica
		//! \return D³ugoœæ tablicy
		template <class T, std::size_t Size>
		static inline std::size_t length(const T(&fixedArray)[Size])
		{
			return Size;
		}

		//! Zwraca rozmiar tablicy w bajtach
		//! \param fixedArray Tablica
		//! \return Rozmiar tablicy
		template <class T>
		static inline std::size_t size(const T & array)
		{
			return length(array) * sizeof(std::remove_reference<decltype(std::declval<const T>()[0])>);
		}

		//! Zwraca rozmiar tablicy w bajtach
		//! \param fixedArray Tablica
		//! \return Rozmiar tablicy
		template <class T, std::size_t Size>
		static inline std::size_t size(const T(&array)[Size])
		{
			return sizeof(array);
		}
	};
}

#endif  // __HEADER_GUARD_UTILS__ARRAY_TRAITS_H__