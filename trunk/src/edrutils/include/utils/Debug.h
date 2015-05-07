/********************************************************************
	created:  2010/08/05
	created:  5:8:2010   18:28
	filename: Debug.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_DEBUG_H__
#define HEADER_GUARD__UTILS_DEBUG_H__

#include <utils/Config.h>
#include <utils/Macros.h>
#include <utils/Utils.h>
#include <sstream>
#include <boost/assert.hpp>
#include <chrono>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

class Debug
{
private:
    //! Klasa bez instancji.
    Debug() {}

public:
    //!
    //! \param file
    //! \param line
    //! \param format
    //! \param
    static void print(const char * file, int line, const char * format, ...);
    //!
    //! \param file
    //! \param line
    //! \param format
    //! \param
    static void formatMessage(char * buffer, int bufferSize, const char * format, ...);
    //! Zaślepka. Pomaga w budowaniu makr.
    //! \param buffer
    //! \param bufferSize
    static void formatMessage(char * buffer, int bufferSize);
};

template<typename TimeT = std::chrono::milliseconds,
	typename ClockT = std::chrono::high_resolution_clock>
struct MeasureDuration
{
	template<typename F, typename ...Args>
	inline static typename TimeT::rep execute(F f, Args&&... args)
	{
		const auto start = ClockT::now();
		
		f(std::forward<Args>(args)...);

		const auto duration = std::chrono::duration_cast<TimeT>(ClockT::now() - start);

		return duration.count();
	}
};

//TODO
//na bazie MeasureDuration dodać logowanie czasu zakresu z nazwami zakresów + wątki

class ScopeTimePrinter
{
public:
	ScopeTimePrinter(const std::string& scopeName);
	ScopeTimePrinter(const std::string& scopeName, const char * file, int line);
	virtual ~ScopeTimePrinter();
private:
	std::string scopeName;
	std::chrono::high_resolution_clock::time_point tick;
};

class MultithreadedScopeTimePrinter : public ScopeTimePrinter
{
public:
	MultithreadedScopeTimePrinter(const std::string& scopeName);
	MultithreadedScopeTimePrinter(const std::string& scopeName, const char * file, int line);
	virtual ~MultithreadedScopeTimePrinter();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

/**
 *	Definicja funkcji wypisującej do konsoli w trybie debug.
 */
#ifdef UTILS_DEBUG
#   define UTILS_DEBUG_PRINT(format, ...) utils::Debug::print(__FILE__, __LINE__, format, __VA_ARGS__)
#else
#   define UTILS_DEBUG_PRINT(...)
#endif

/**
 *	Definicja asercji.
 */
#ifdef UTILS_DEBUG

#define UTILS_ASSERT(condition, ...) BOOST_ASSERT_MSG(condition, COMA_AWARE_STRINGIZE(#condition,__VA_ARGS__))
#define UTILS_FAIL(...) UTILS_ASSERT(false, COMA_AWARE_STRINGIZE(failure,__VA_ARGS__))

#else
#   define UTILS_ASSERT(...)
#   define UTILS_FAIL(...)
#endif




#endif  // HEADER_GUARD__UTILS_DEBUG_H__
