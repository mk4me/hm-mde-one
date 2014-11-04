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
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <sstream>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

// Sugerowany początek makra składającego się z wielu instrukcji
#define UTILS_MULTISTATEMENT_BEGIN  do {

// Sugerowany koniec makra składającego się z wielu instrukcji
#ifdef _MSC_VER
#   define UTILS_MULTISTATEMENT_END     \
    __pragma(warning(push))             \
    __pragma(warning(disable: 4127))    \
    } while(0) /* brak ; na koncu! */   \
    __pragma(warning(pop))
#else
#   define UTILS_MULTISTATEMENT_END    } while(0) /* brak ; na koncu! */
#endif

//------------------------------------------------------------------------------

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

//! Zwalnia zadany wskaźnik oraz zeruje jego wartość.
#define UTILS_DELETEPTR(ptr) UTILS_MULTISTATEMENT_BEGIN \
    delete ptr; \
    ptr = nullptr; \
    UTILS_MULTISTATEMENT_END

//! Zwalnia zadany wskaźnik oraz zeruje jego wartość.
//! \param ptr Wskaźnik do zwolnienia.
template <class T>
inline void deletePtr(T *& ptr)
{
    delete ptr;
    ptr = nullptr;
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

//------------------------------------------------------------------------------

//! Zwraca znak liczby. Używane do uproszczenia mnożenia.
//! \param value
//! \return 1 albo -1
template <class T, size_t Size>
inline size_t length(const T (&fixedArray)[Size])
{
  return Size;
}


//------------------------------------------------------------------------------

struct NullType {};

//------------------------------------------------------------------------------

//! Konwersja danego typu do łańcucha.
//! \param source
template <class T>
inline std::string toString(const T& source)
{
    return boost::lexical_cast<std::string>(source);
}

//------------------------------------------------------------------------------

//! \param stream Strumień którego rozmiar pobieramy
//! \return Rozmiar strumienia
inline static const std::streamsize streamSize(std::istream & stream)
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
inline static const std::string readStream(std::istream & stream)
{
	unsigned int BufferSize = 1024 * 512;
	std::unique_ptr<char[]> buffer(new char[BufferSize] {0});
	std::string ret;

	std::streamsize read = 0;
	while ((read = forceReadSome(stream, buffer.get(), BufferSize)) > 0) { ret.append(buffer.get(), read); }
	return ret;
}

//------------------------------------------------------------------------------

//! \tparam T Typ dla któego wykonujemy swap bajtów
//! \param value Wartość dla któej wykonujemy swap bajtów
//! \return Wartość ze zmienioną kolejnością bajtów
template<typename T>
inline const T EndianSwap(const T value)
{
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = value;

	for (std::size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
};

//------------------------------------------------------------------------------

//! \tparam Typ uywany do czyszczenia przy niszczeniu - funktor z operatorem ()
class Cleanup
{
public:
	typedef std::function<void()> Functor;
public:
	//! \param cleanup Obiekt uywany do czyszczenia
	Cleanup(const Functor & cleanup) : cleanup(cleanup) {}

	//! \param cleanup Obiekt uywany do czyszczenia
	template<typename T>
	Cleanup(const T & cleanup) : cleanup(cleanup) {}

	//! Destruktor
	~Cleanup() { cleanup(); }
private:
	//! Obiekt czyszczacy
	Functor cleanup;
};

//------------------------------------------------------------------------------

//! \tparam T Typ kolekcji nieuporządkowanej poddawanej łączeniu
//! \param a Pierwsza kolekcja łączona
//! \param b Druga kolekcja łączona
//! \return Kolekcja będąca rezultatem dołączonia do kolekcji a kolekcji b
template<class T>
const T mergeUnordered(const T & a, const T & b)
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
const T mergeOrdered(const T & a, const T & b)
{
	T ret(a);
	ret.insert(b.begin(), b.end());
	return ret;
}


//------------------------------------------------------------------------------
/**
 *	Definicja ostrzeżenia o przestarzałej funkcjonalności
 */
#ifdef __GNUC__
#   define UTILS_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#   define UTILS_DEPRECATED(func) __declspec(deprecated) func
#else
#   pragma basicMessage("WARNING: You need to implement DEPRECATED for this compiler")
#   define UTILS_DEPRECATED(func) func
#endif

//------------------------------------------------------------------------------
//! Makro pozwalające generować string z ciągu strumieni
#define UTILS_FORMAT_STRING(ITEMS)												\
  ( ( dynamic_cast<std::ostringstream &> (										\
         std::ostringstream() . seekp( 0, std::ios_base::cur ) << ITEMS )		\
    ) . str() )

//------------------------------------------------------------------------------
/**
 *	Makra zapamiętujące/odtwarzające stan warningów.
 */
#if defined(_MSC_VER)
#   define UTILS_PUSH_WARNINGS  __pragma(warning(push))
#   define UTILS_POP_WARNINGS   __pragma(warning(pop))
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING __pragma(warning(disable: 4251))
#elif defined(__GNUC__) && defined(UTILS_CXX0X)
#   define DO_PRAGMA(x) _Pragma (#x)
#   define UTILS_PUSH_WARNINGS  DO_PRAGMA(GCC diagnostic push)
#   define UTILS_POP_WARNINGS   DO_PRAGMA(GCC diagnostic pop)
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING
#else
#   pragma basicMessage("WARNING: You need to implement _Pragma for this compiler")
#   define UTILS_PUSH_WARNINGS
#   define UTILS_POP_WARNINGS
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING
#endif


//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__UTILS_H__
