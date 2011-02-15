/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   16:48
	filename: Ptr.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__SMARTPTR_H__
#define __HEADER_GUARD_CORE__SMARTPTR_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>

// #define CORE_SMART_PTR_TEMPLATE boost::shared_ptr
// #define CORE_SHARED_PTR(type) boost::shared_ptr<type>
// #define CORE_CONST_SHARED_PTR(type) boost::shared_ptr<const type>
// #define CORE_WEAK_PTR(type) boost::weak_ptr<type>
// #define CORE_CONST_WEAK_PTR(type) boost::weak_ptr<const type>
// 
// using boost::dynamic_pointer_cast;
// using boost::static_pointer_cast;

#include <utils/Macros.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    using boost::shared_ptr;
    using boost::weak_ptr;
    using boost::dynamic_pointer_cast;
    using boost::static_pointer_cast;
    using boost::make_shared;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__SMARTPTR_H__