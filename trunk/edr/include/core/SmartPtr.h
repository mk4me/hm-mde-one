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
#include <boost/scoped_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    using boost::shared_ptr;
    using boost::weak_ptr;
    using boost::scoped_ptr;
    using boost::dynamic_pointer_cast;
    using boost::static_pointer_cast;
    using boost::const_pointer_cast;
    using boost::make_shared;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__SMARTPTR_H__