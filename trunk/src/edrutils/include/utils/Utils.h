/********************************************************************
	created:    2010/05/14
	created:    14:5:2010     10:13
	filename: Utils.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_H__
#define HEADER_GUARD__UTILS_H__

#include <utils/Config.h>
#include <utils/Macros.h>
#include <utils/SmartPtr.h>
#include <cstring>
#include <sstream>
#include <type_traits>
#include <memory>
#include <functional>
#include <cstdint>
#include <utility>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

template<typename T>
T correctNegativeZero(const T value)
{
	static_assert(std::is_floating_point<T>::value, "T must be floating point type");
	if (value == T()){
		return std::fabs(value);
	}

	return value;
}

//
template <class T>
inline T clamp(const T& v, const T& min, const T& max)
{
    return ( v < min ) ? min : ((v > max) ? max : v);
}

//------------------------------------------------------------------------------

//! Zeruje zadany obiekt.
//! \param object Obiekt do wyzerowania.
template <class T>
inline void zero(T & object)
{
    memset(&object, 0, sizeof(T));
}

//------------------------------------------------------------------------------
template<class T> inline
typename std::decay<T>::type decay_copy(T && val)
{ // forward val as value of decayed type
return (std::forward<T>(val));
}

//! Zwalnia zadany wskaźnik oraz zeruje jego wartość.
//! \param ptr Wskaźnik do zwolnienia.
template <class T>
inline void deletePtr(T *& ptr)
{
	UTILS_DELETEPTR(ptr);
}

//------------------------------------------------------------------------------

//! Zwraca znak liczby. Używane do uproszczenia mnożenia.
//! \param value
//! \return 1 albo -1
template <class T>
inline T sign(T value)
{
    return value >= 0 ? T(1) : T(-1);
}

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
		static inline decltype(std::declval<const T>()[Element]) extract(const T & array)
		{
			return array[Element];
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam Size Rozmiar tablicy
		template <typename T, std::size_t Size>
		//! \param array Tablica
		//! \return Wypakowany element
		static inline T extract(const T(&array)[Size])
		{
			static_assert(Element < Size, "Element index out of range");
			return array[Element];
		}
	};

	//! Klasa pozwalaj�ca wypakowywa� dane z wektor�w
	class ElementExtractor
	{
	public:
		//! \param idx Index obiketu kt�ry chcemy wypakowywa� z wektora
		ElementExtractor(const std::size_t idx) : idx(idx) {}
		//! \param Other Inny kopiowany extractor
		ElementExtractor(const ElementExtractor & Other) : idx(Other.idx) {}
		//! Destruktor
		~ElementExtractor() {}

		//! \tparam T Typ tablicy
		template <typename T>
		//! \param array Tablica
		//! \return Wypakowany element
		static inline decltype(std::declval<const T>()[0]) extract(const T & array)
		{
			return array[idx];
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam Size Rozmiar tablicy
		template <typename T, std::size_t Size>
		//! \param array Tablica
		//! \return Wypakowany element
		static inline T extract(const T(&array)[Size])
		{			
			return array[idx];
		}

	private:
		//! Indeks spod kt�rego wybieramy dane z wektora
		const std::size_t idx;
	};

	//! Zwraca długość tablicy
	//! \param fixedArray Tablica
	//! \return Długość tablicy
	template <class T>
	static inline std::size_t length(const T & array)
	{
		return array.size();
	}

	//! Zwraca długość tablicy
	//! \param fixedArray Tablica
	//! \return Długość tablicy
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

//------------------------------------------------------------------------------

struct NullType {};

//------------------------------------------------------------------------------

struct StreamTools
{
	//! \param stream Strumień którego rozmiar pobieramy
	//! \return Rozmiar strumienia
	inline static const std::streamsize size(std::istream & stream)
	{
		const auto pos = stream.tellg();
		stream.seekg(0, std::ios::beg);
		const auto start = stream.tellg();
		stream.seekg(0, std::ios::end);
		const auto end = stream.tellg();
		stream.seekg(pos);

		return std::streamsize(end - start);
	}

	//------------------------------------------------------------------------------

	//! \param stream Strumień do czytania
	//! \param out Bufor do któego czytamy strumień
	//! \param bytesCount Ilość bajtów do przeczytania ze strumienia
	//! \return Faktyczna ilośc bajtów przeczytana ze strumienia
	inline static const std::streamsize forceReadSome(std::istream & stream,
		char * out, const std::streamsize bytesCount)
	{
		const auto state = stream.rdstate();
		stream.read(out, bytesCount);
		const std::streamsize bytes = stream.gcount();
		stream.clear(state | (stream.eof() == true ? std::ios_base::eofbit : std::ios_base::goodbit));
		return bytes;
	}

	//------------------------------------------------------------------------------

	//! \param stream Strumień do skopiowania
	//! \return Strumień zapisany w stringu
	inline static std::string read(std::istream & stream, void * buffer,
		const std::size_t bufferSize)
	{
		std::string ret;

		std::streamsize read = 0;
		while ((read = forceReadSome(stream, static_cast<char*>(buffer), bufferSize)) > 0) { ret.append(static_cast<char*>(buffer), static_cast<unsigned int>(read)); }
		return ret;
	}

	//------------------------------------------------------------------------------

	//! \param stream Strumień do skopiowania
	//! \return Strumień zapisany w stringu
	inline static std::string read(std::istream & stream)
	{
		unsigned int BufferSize = 1024 * 512;
		std::unique_ptr<char[]> buffer(new char[BufferSize] {0});
		return read(stream, buffer.get(), BufferSize);
	}

	template<typename T, unsigned int Size>
	//! \param stream Strumień do skopiowania
	//! \return Strumień zapisany w stringu
	inline static std::string read(std::istream & stream, T(&fixedArray)[Size])
	{
		return read(stream, (void*)fixedArray, ArrayTraits::size(fixedArray));
	}
};

//------------------------------------------------------------------------------

//! \tparam T Typ dla któego wykonujemy swap bajtów
//! \param value Wartość dla któej wykonujemy swap bajtów
//! \return Wartość ze zmienioną kolejnością bajtów
template<typename T>
inline T EndianSwap(const T value)
{
	union
	{
		T u;
		uint8_t u8[sizeof(T)];
	} es;

	es.u = value;

	std::reverse(es.u8, es.u8 + sizeof(T));

	return es.u;
};

//------------------------------------------------------------------------------

class Cleanup
{
	private:

		class CleanupPrivateBase
		{
		public:
			virtual ~CleanupPrivateBase() {}
		};

		template<typename T>
		class CleanupPrivate : public CleanupPrivateBase
		{
		public:
			CleanupPrivate(T && t) : t(std::move(t)) {}
			CleanupPrivate(const T & t) : t(t) {}
			virtual ~CleanupPrivate() { t(); }
		private:
			T t;
		};

public:	

	//! \param cleanup Obiekt uywany do czyszczenia
	template<typename T>
	Cleanup(const T & t) : cleanup(utils::make_shared<CleanupPrivate<T>>(t)) {}

	//! \param cleanup Obiekt uywany do czyszczenia
	template<typename T>
	Cleanup(T && t) : cleanup(utils::make_shared<CleanupPrivate<T>>(std::move(t))) {}

	//! Destruktor
	~Cleanup() {}
private:
	//! Obiekt czyszczacy
	utils::shared_ptr<CleanupPrivateBase> cleanup;
};

//------------------------------------------------------------------------------

//! \tparam T Typ kolekcji nieuporządkowanej poddawanej łączeniu
//! \param a Pierwsza kolekcja łączona
//! \param b Druga kolekcja łączona
//! \return Kolekcja będąca rezultatem dołączonia do kolekcji a kolekcji b
template<class T>
T mergeUnordered(const T & a, const T & b)
{
	T ret(a);
	ret.insert(ret.end(), b.begin(), b.end());
	return ret;
}

//------------------------------------------------------------------------------

//! \tparam T Typ kolekcji uporządkowanej poddawanej łączeniu
//! \param a Pierwsza kolekcja łączona
//! \param b Druga kolekcja łączona
//! \return Kolekcja będąca rezultatem dołączonia elementó kolekcji b do elementów kolekcji kolekcji a
template<class T>
T mergeOrdered(const T & a, const T & b)
{
	T ret(a);
	ret.insert(b.begin(), b.end());
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__UTILS_H__
