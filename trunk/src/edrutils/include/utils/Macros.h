/********************************************************************
created:  2010/04/23
created:  23:4:2010   7:58
filename: Macros.h
author:	  Piotr Gwiazdowski

purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__MACROS_H__
#define HEADER_GUARD_UTILS__MACROS_H__

//------------------------------------------------------------------------------
#if _MSC_VER >= 1600 || (defined(__GNUC__) && (__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)))
	#define UTILS_CXX0X
#else
	#error "Unsupported compiler - C++11 support required"
#endif

//------------------------------------------------------------------------------
// Visual studio jeszcze nie wspiera noexcept, stad to makro
#ifndef _MSC_VER
	#define NOEXCEPT noexcept
#else
	#define NOEXCEPT
#endif

//------------------------------------------------------------------------------
// Sugerowany początek makra składającego się z wielu instrukcji
#define UTILS_MULTISTATEMENT_BEGIN  do {

//------------------------------------------------------------------------------
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
//! Zwalnia zadany wskaźnik oraz zeruje jego wartość.
#define UTILS_DELETEPTR(ptr) UTILS_MULTISTATEMENT_BEGIN \
    delete ptr; \
    ptr = nullptr; \
    UTILS_MULTISTATEMENT_END

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
// Makro podstawiające nazwę aktualnej funkcji
//TODO
//użyć BOOST_CURRENT_FUNCTION (boost/current_function.hpp)
#if defined __GNUC__
	#define __UTILS_PORTABLE_FUNCTION_NAME __func__
#elif defined _MSC_VER
	#define __UTILS_PORTABLE_FUNCTION_NAME __FUNCTION__
#else
	#error Provide macro name for retriving currently processed function name for current compiler!
#endif

//------------------------------------------------------------------------------
#if defined(_DEBUG) || defined(DEBUG)
#define UTILS_DEBUG
#endif

//------------------------------------------------------------------------------
// Makro zamieniająca parametr na łańcuch.
#define STRINGIZE(arg)  __STRINGIZE(arg)
#define __STRINGIZE(arg) #arg

//------------------------------------------------------------------------------
// Makro łączące dwa parametry.
#define CONCATENATE(arg1, arg2)   __CONCATENATE(arg1, arg2)
#define __CONCATENATE(arg1, arg2)  arg1##arg2

//------------------------------------------------------------------------------
// Makro zamieniające definicję zawierającą przecinki na łańcuch.
#ifdef __GNUC__
// Dla gcc parametry trzeba "rozbić"
#define COMA_AWARE_STRINGIZE(...) # __VA_ARGS__ //__FOR_EACH(STRINGIZE, ",", __VA_ARGS__)
#else // __GNUC__
// Liczymy na to, że sobie poradzi (tak jak MSVC)
#define COMA_AWARE_STRINGIZE(...) __STRINGIZE(__VA_ARGS__)
#endif // __GNUC__

//------------------------------------------------------------------------------
#ifdef __GNUC__
// Makro zwracające liczbę parametrow. Obecnie dzialający zakres to 0-63
#define NARG(...) __NARG(__VA_ARGS__, __NARG_RSEQ_N())
#define __NARG(...) __NARG_ARG_N(__VA_ARGS__)

// Parametry. Makro dziala w ten sposob, ze przekazując mu __VA_ARGS__ oraz
// indeksy w odwrotnej kolejności dostaniemy liczność tych pierwszych.
#define __NARG_ARG_N( \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,N,...) N

// Parametry w odwrotnej kolejności
#define __NARG_RSEQ_N() \
    63,62,61,60,                   \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
    9,8,7,6,5,4,3,2,1,0
#endif // __GNUC__

//------------------------------------------------------------------------------

#ifdef __GNUC__

// Makro wywołujące makro what dla każdego z parametrów w __VA_ARGS__.
// Obecnie wspiera od 1 do 63 parametrów
#define __FOR_EACH(what, separator, ...) __FOR_EACH_N(NARG(__VA_ARGS__), what, separator, __VA_ARGS__)
#define __FOR_EACH_N(N, what, separator, x, ...) CONCATENATE(__FOR_EACH_, N)(what, separator, x, __VA_ARGS__)

// Makrodefinicje "rozbijające" parametry na pojedyncze wartości
#define __FOR_EACH_1(what, separator, x, ...)  what(x)
#define __FOR_EACH_2(what, separator, x, ...)  what(x) separator __FOR_EACH_1(what, separator, __VA_ARGS__);
#define __FOR_EACH_3(what, separator, x, ...)  what(x) separator __FOR_EACH_2(what, separator, __VA_ARGS__);
#define __FOR_EACH_4(what, separator, x, ...)  what(x) separator __FOR_EACH_3(what, separator, __VA_ARGS__);
#define __FOR_EACH_5(what, separator, x, ...)  what(x) separator __FOR_EACH_4(what, separator, __VA_ARGS__);
#define __FOR_EACH_6(what, separator, x, ...)  what(x) separator __FOR_EACH_5(what, separator, __VA_ARGS__);
#define __FOR_EACH_7(what, separator, x, ...)  what(x) separator __FOR_EACH_6(what, separator, __VA_ARGS__);
#define __FOR_EACH_8(what, separator, x, ...)  what(x) separator __FOR_EACH_7(what, separator, __VA_ARGS__);
#define __FOR_EACH_9(what, separator, x, ...)  what(x) separator __FOR_EACH_8(what, separator, __VA_ARGS__);
#define __FOR_EACH_10(what, separator, x, ...) what(x) separator __FOR_EACH_9(what, separator, __VA_ARGS__);
#define __FOR_EACH_11(what, separator, x, ...) what(x) separator __FOR_EACH_10(what, separator, __VA_ARGS__);
#define __FOR_EACH_12(what, separator, x, ...) what(x) separator __FOR_EACH_11(what, separator, __VA_ARGS__);
#define __FOR_EACH_13(what, separator, x, ...) what(x) separator __FOR_EACH_12(what, separator, __VA_ARGS__);
#define __FOR_EACH_14(what, separator, x, ...) what(x) separator __FOR_EACH_13(what, separator, __VA_ARGS__);
#define __FOR_EACH_15(what, separator, x, ...) what(x) separator __FOR_EACH_14(what, separator, __VA_ARGS__);
#define __FOR_EACH_16(what, separator, x, ...) what(x) separator __FOR_EACH_15(what, separator, __VA_ARGS__);
#define __FOR_EACH_17(what, separator, x, ...) what(x) separator __FOR_EACH_16(what, separator, __VA_ARGS__);
#define __FOR_EACH_18(what, separator, x, ...) what(x) separator __FOR_EACH_17(what, separator, __VA_ARGS__);
#define __FOR_EACH_19(what, separator, x, ...) what(x) separator __FOR_EACH_18(what, separator, __VA_ARGS__);
#define __FOR_EACH_20(what, separator, x, ...) what(x) separator __FOR_EACH_19(what, separator, __VA_ARGS__);
#define __FOR_EACH_21(what, separator, x, ...) what(x) separator __FOR_EACH_20(what, separator, __VA_ARGS__);
#define __FOR_EACH_22(what, separator, x, ...) what(x) separator __FOR_EACH_21(what, separator, __VA_ARGS__);
#define __FOR_EACH_23(what, separator, x, ...) what(x) separator __FOR_EACH_22(what, separator, __VA_ARGS__);
#define __FOR_EACH_24(what, separator, x, ...) what(x) separator __FOR_EACH_23(what, separator, __VA_ARGS__);
#define __FOR_EACH_25(what, separator, x, ...) what(x) separator __FOR_EACH_24(what, separator, __VA_ARGS__);
#define __FOR_EACH_26(what, separator, x, ...) what(x) separator __FOR_EACH_25(what, separator, __VA_ARGS__);
#define __FOR_EACH_27(what, separator, x, ...) what(x) separator __FOR_EACH_26(what, separator, __VA_ARGS__);
#define __FOR_EACH_28(what, separator, x, ...) what(x) separator __FOR_EACH_27(what, separator, __VA_ARGS__);
#define __FOR_EACH_29(what, separator, x, ...) what(x) separator __FOR_EACH_28(what, separator, __VA_ARGS__);
#define __FOR_EACH_30(what, separator, x, ...) what(x) separator __FOR_EACH_29(what, separator, __VA_ARGS__);
#define __FOR_EACH_31(what, separator, x, ...) what(x) separator __FOR_EACH_30(what, separator, __VA_ARGS__);
#define __FOR_EACH_32(what, separator, x, ...) what(x) separator __FOR_EACH_31(what, separator, __VA_ARGS__);
#define __FOR_EACH_33(what, separator, x, ...) what(x) separator __FOR_EACH_32(what, separator, __VA_ARGS__);
#define __FOR_EACH_34(what, separator, x, ...) what(x) separator __FOR_EACH_33(what, separator, __VA_ARGS__);
#define __FOR_EACH_35(what, separator, x, ...) what(x) separator __FOR_EACH_34(what, separator, __VA_ARGS__);
#define __FOR_EACH_36(what, separator, x, ...) what(x) separator __FOR_EACH_35(what, separator, __VA_ARGS__);
#define __FOR_EACH_37(what, separator, x, ...) what(x) separator __FOR_EACH_36(what, separator, __VA_ARGS__);
#define __FOR_EACH_38(what, separator, x, ...) what(x) separator __FOR_EACH_37(what, separator, __VA_ARGS__);
#define __FOR_EACH_39(what, separator, x, ...) what(x) separator __FOR_EACH_38(what, separator, __VA_ARGS__);
#define __FOR_EACH_40(what, separator, x, ...) what(x) separator __FOR_EACH_39(what, separator, __VA_ARGS__);
#define __FOR_EACH_41(what, separator, x, ...) what(x) separator __FOR_EACH_40(what, separator, __VA_ARGS__);
#define __FOR_EACH_42(what, separator, x, ...) what(x) separator __FOR_EACH_41(what, separator, __VA_ARGS__);
#define __FOR_EACH_43(what, separator, x, ...) what(x) separator __FOR_EACH_42(what, separator, __VA_ARGS__);
#define __FOR_EACH_44(what, separator, x, ...) what(x) separator __FOR_EACH_43(what, separator, __VA_ARGS__);
#define __FOR_EACH_45(what, separator, x, ...) what(x) separator __FOR_EACH_44(what, separator, __VA_ARGS__);
#define __FOR_EACH_46(what, separator, x, ...) what(x) separator __FOR_EACH_45(what, separator, __VA_ARGS__);
#define __FOR_EACH_47(what, separator, x, ...) what(x) separator __FOR_EACH_46(what, separator, __VA_ARGS__);
#define __FOR_EACH_48(what, separator, x, ...) what(x) separator __FOR_EACH_47(what, separator, __VA_ARGS__);
#define __FOR_EACH_49(what, separator, x, ...) what(x) separator __FOR_EACH_48(what, separator, __VA_ARGS__);
#define __FOR_EACH_50(what, separator, x, ...) what(x) separator __FOR_EACH_49(what, separator, __VA_ARGS__);
#define __FOR_EACH_51(what, separator, x, ...) what(x) separator __FOR_EACH_50(what, separator, __VA_ARGS__);
#define __FOR_EACH_52(what, separator, x, ...) what(x) separator __FOR_EACH_51(what, separator, __VA_ARGS__);
#define __FOR_EACH_53(what, separator, x, ...) what(x) separator __FOR_EACH_52(what, separator, __VA_ARGS__);
#define __FOR_EACH_54(what, separator, x, ...) what(x) separator __FOR_EACH_53(what, separator, __VA_ARGS__);
#define __FOR_EACH_55(what, separator, x, ...) what(x) separator __FOR_EACH_54(what, separator, __VA_ARGS__);
#define __FOR_EACH_56(what, separator, x, ...) what(x) separator __FOR_EACH_55(what, separator, __VA_ARGS__);
#define __FOR_EACH_57(what, separator, x, ...) what(x) separator __FOR_EACH_56(what, separator, __VA_ARGS__);
#define __FOR_EACH_58(what, separator, x, ...) what(x) separator __FOR_EACH_57(what, separator, __VA_ARGS__);
#define __FOR_EACH_59(what, separator, x, ...) what(x) separator __FOR_EACH_58(what, separator, __VA_ARGS__);
#define __FOR_EACH_60(what, separator, x, ...) what(x) separator __FOR_EACH_59(what, separator, __VA_ARGS__);
#define __FOR_EACH_61(what, separator, x, ...) what(x) separator __FOR_EACH_60(what, separator, __VA_ARGS__);
#define __FOR_EACH_62(what, separator, x, ...) what(x) separator __FOR_EACH_61(what, separator, __VA_ARGS__);
#define __FOR_EACH_63(what, separator, x, ...) what(x) separator __FOR_EACH_62(what, separator, __VA_ARGS__);

#endif // __GNUC__

//------------------------------------------------------------------------------

#endif  // HEADER_GUARD_UTILS__MACROS_H__
