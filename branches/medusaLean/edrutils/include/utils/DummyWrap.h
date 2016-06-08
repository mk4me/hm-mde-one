/********************************************************************
	created:  2016/01/20
	filename: DummyWrap.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__DUMMYWRAP_H__
#define __HEADER_GUARD_UTILS__DUMMYWRAP_H__

#include <type_traits>
#include <utils/SmartPtr.h>

namespace utils
{
	//! \tparam T Typ dla kt�rego tworzymy "pusty" deleter
	template<typename T>
	struct DummyDefaultDelete
	{	// default deleter for unique_ptr
		//! Konstruktor domyslny
		DummyDefaultDelete() throw()
		{	// default construct
		}

		//! \tparam T2 Typ z kt�rego pr�bujemy konwertowa� na typ bazowy
		template<class T2,
		class = typename std::enable_if<std::is_convertible<T2 *, T *>::value>::type>
			//! Konstruktor kopiuj�cy
			DummyDefaultDelete(const DummyDefaultDelete<T2>&) throw()
		{	// construct from another default_delete
		}

		//! Operator usuwaj�cy pusty
		//! \param
		void operator()(T*) const throw()
		{
		}
	};

	//------------------------------------------------------------------------------

	//! \tparam T Typ dla kt�rego tworzymy dummy wrapper
	template<typename T>
	//! \param t Wrappowany wska�nik
	//! \return Inteligenty wska�nik nic nie usuwaj�cy
	utils::shared_ptr<T> dummyWrap(T* t)
	{
		return utils::shared_ptr<T>(t, DummyDefaultDelete<T>());
	}
}

#endif  // __HEADER_GUARD_UTILS__DUMMYWRAP_H__