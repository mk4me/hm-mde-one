/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   16:48
	filename: Ptr.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__SMARTPTR_H__
#define __HEADER_GUARD_UTILS__SMARTPTR_H__

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
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
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#define DEFINE_SMART_POINTERS_EXT(className, pointerName)                    \
	typedef utils::shared_ptr<className> pointerName ## Ptr;                 \
	typedef utils::shared_ptr<const className> pointerName ## ConstPtr;      \
	typedef utils::weak_ptr<className> pointerName ## WeakPtr;               \
	typedef utils::weak_ptr<const className> pointerName ## ConstWeakPtr;

#define DEFINE_SMART_POINTERS(className)                                  \
	DEFINE_SMART_POINTERS_EXT(className, className)

#define DEFINE_SCOPED_SMART_POINTERS(className, targetNamespace)                                    \
	typedef utils::shared_ptr<className> targetNamespace ## :: ## className ## Ptr;                  \
	typedef utils::shared_ptr<const className> targetNamespace ## :: ## className ## ConstPtr;       \
	typedef utils::weak_ptr<className> targetNamespace ## :: ## className ## WeakPtr;                \
	typedef utils::weak_ptr<const className> targetNamespace ## :: ## className ## ConstWeakPtr;

#endif  // __HEADER_GUARD_CORE__SMARTPTR_H__
