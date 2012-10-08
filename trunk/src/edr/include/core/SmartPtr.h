/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   16:48
	filename: Ptr.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__SMARTPTR_H__
#define __HEADER_GUARD_CORE__SMARTPTR_H__

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

	using boost::enable_shared_from_this;
	using boost::shared_array;
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

#define DEFINE_SMART_POINTERS(className)                                  \
    typedef core::shared_ptr<className> className ## Ptr;                 \
    typedef core::shared_ptr<const className> className ## ConstPtr;      \
    typedef core::weak_ptr<className> className ## WeakPtr;               \
    typedef core::weak_ptr<const className> className ## ConstWeakPtr;

#define DEFINE_SCOPED_SMART_POINTERS(className, targetNamespace)                                    \
    typedef core::shared_ptr<className> targetNamespace ## :: ## className ## Ptr;                  \
    typedef core::shared_ptr<const className> targetNamespace ## :: ## className ## ConstPtr;       \
    typedef core::weak_ptr<className> targetNamespace ## :: ## className ## WeakPtr;                \
    typedef core::weak_ptr<const className> targetNamespace ## :: ## className ## ConstWeakPtr;

#endif  // __HEADER_GUARD_CORE__SMARTPTR_H__
