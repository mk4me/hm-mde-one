/********************************************************************
	created:  2011/02/15
	created:  15:2:2011   9:41
	filename: PtrPolicyHelper.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__PTRPOLICYHELPER_H__
#define HEADER_GUARD_UTILS__PTRPOLICYHELPER_H__

#include <utils/Macros.h>

#ifdef UTILS_CXX0X
#   include <type_traits>
#else
#   include <boost/type_traits.hpp>
#endif

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

#ifdef UTILS_CXX0X
    namespace __traits = ::std;
#else
    namespace __traits = ::boost;
#endif

    template <class T> struct is_ptr_policy : public __traits::false_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD_UTILS__PTRPOLICYHELPER_H__