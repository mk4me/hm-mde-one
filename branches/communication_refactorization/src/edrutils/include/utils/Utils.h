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
#include <boost/function.hpp>
#include <cstring>

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
#define UTIL_DELETEPTR(ptr) UTILS_MULTISTATEMENT_BEGIN \
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

//! Tworzy std::string z typu const char*. 
//! \param ptr Łańcuch, może być nullptr
//! \return Łańcuch na podstawie parametru lub pusty string, gdy przekazano nullptr
inline std::string safeString(const char* ptr)
{
	return ptr ? std::string(ptr) : std::string();
}

//------------------------------------------------------------------------------

//! \param stream Strumień do skopiowania
//! \return Plik zapisany w stringu
inline static const std::string readStream(std::istream * stream)
{
	std::string ret;
	static const unsigned int BufferSize = 1024 * 512;
	char buffer[BufferSize] = { 0 };

	//TODO - uruchomić metody strumienia do szybkiego czytania danych a nie iteratorami
	int read = 0;
	while ((read = stream->readsome(buffer, BufferSize)) > 0) { ret.append(buffer, read); }
	return ret;
}

//------------------------------------------------------------------------------

//! \tparam Typ uywany do czyszczenia przy niszczeniu - funktor z operatorem ()
class Cleanup
{
public:
	typedef boost::function<void()> Functor;
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
