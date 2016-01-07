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
#include <csetjmp>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////	

template<typename T>
struct DummyDefaultDelete
{	// default deleter for unique_ptr

	DummyDefaultDelete() throw()
	{	// default construct
	}

	template<class T2,
	class = typename std::enable_if<std::is_convertible<T2 *, T *>::value,
		void>::type>
		DummyDefaultDelete(const DummyDefaultDelete<T2>&) throw()
	{	// construct from another default_delete
	}

	void operator()(T *_Ptr) const throw()
	{						
	}
};

template<typename T>
utils::shared_ptr<T> dummyWrap(T* t)
{
	return utils::shared_ptr<T>(t, DummyDefaultDelete<T>());
}

//! \tparam T Typ wartości zmiennoprzecinkowej 
template<typename T>
//! \param value Wartość dla której eliminujemy ujemne zero
//! \return Wartość nieujemna
static inline T correctNegativeZero(const T value)
{
	static_assert(std::is_floating_point<T>::value, "T must be floating point type");
	if (value == T(0)){
		return std::fabs(value);
	}

	return value;
}

//! \param T Typ clampowanej wartości
template <class T>
//! \param v Clampowana wartość
//! \param min Minimalna wartość
//! \param max Maksymalna wartość
//! \return Wartość w zadanym przedziale
static inline T clamp(const T& v, const T& min, const T& max)
{
    return ( v < min ) ? min : ((v > max) ? max : v);
}

//------------------------------------------------------------------------------

//! Zeruje zadany obiekt.
//! \tparam T Typ zerowanego obiektu
template <class T>
//! \param object Obiekt do wyzerowania.
static inline void zero(T & object)
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
//! \tparam T Typ wskaźnika
template <class T>
//! \param ptr Wskaźnik do zwolnienia.
static inline void deletePtr(T *& ptr)
{
	UTILS_DELETEPTR(ptr);
}

//------------------------------------------------------------------------------
//! \tparam Typ wartości dla której wyznaczamy znak
template <typename T>
//! \param value Wartość dla której wyciągamy znak
//! \return 1 albo -1 w zależności od znaku
static inline int sign(const T x, std::false_type)
{
	return T(0) < x;
}

//! \tparam Typ wartości dla której wyznaczamy znak
template <typename T>
//! \param value Wartość dla której wyciągamy znak
//! \return 1 albo -1 w zależności od znaku
static inline int sign(const T x, std::true_type)
{
	return (T(0) < x) - (x < T(0));
}

//! \tparam Typ wartości dla której wyznaczamy znak
template <typename T>
//! \param value Wartość dla której wyciągamy znak
//! \return 1 albo -1 w zależności od znaku
static inline int sign(const T x)
{
	return sign(x, std::is_signed<T>());
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

	//! Klasa pozwalająca wypakowywać dane z wektorów
	class ElementExtractor
	{
	public:
		//! \param idx Index obiketu który chcemy wypakowywać z wektora
		ElementExtractor(const std::size_t idx) : idx(idx) {}
		//! \param Other Inny kopiowany extractor
		ElementExtractor(const ElementExtractor & Other) : idx(Other.idx) {}
		//! Destruktor
		~ElementExtractor() {}

		//! \tparam T Typ tablicy
		template <typename T>
		//! \param array Tablica
		//! \return Wypakowany element
		inline decltype(std::declval<const T>()[0]) extract(const T & array)
		{
			return array[idx];
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam Size Rozmiar tablicy
		template <typename T, std::size_t Size>
		//! \param array Tablica
		//! \return Wypakowany element
		inline T extract(const T(&array)[Size])
		{			
			return array[idx];
		}

	private:
		//! Indeks spod którego wybieramy dane z wektora
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
		char buffer[1024 * 512] = { 0 };
		//unsigned int BufferSize = 1024 * 512;
		//std::unique_ptr<char[]> buffer(new char[BufferSize] {0});
		//return read(stream, buffer.get(), BufferSize);
		return read(stream, buffer);
	}

	template<typename T, unsigned int Size>
	//! \param stream Strumień do skopiowania
	//! \return Strumień zapisany w stringu
	inline static std::string read(std::istream & stream, T(&fixedArray)[Size])
	{
		return read(stream, (void*)fixedArray, ArrayTraits::size(fixedArray));
	}
};

class StringStreamBufferGrabber : public std::stringstream
{
public:
	StringStreamBufferGrabber(std::ostream & os) : old(nullptr), os(os)
	{
		old = os.rdbuf(rdbuf());
	}

	~StringStreamBufferGrabber()
	{
		os.rdbuf(old);
	}

private:
	std::streambuf* old;
	std::ostream & os;
};

//------------------------------------------------------------------------------

//! \tparam T Typ dla któego wykonujemy swap bajtów
//! \param value Wartość dla któej wykonujemy swap bajtów
//! \return Wartość ze zmienioną kolejnością bajtów
template<typename T>
static inline T EndianSwap(const T value)
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
static inline T mergeUnordered(const T & a, const T & b)
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
static inline T mergeOrdered(const T & a, const T & b)
{
	T ret(a);
	ret.insert(b.begin(), b.end());
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__UTILS_H__
