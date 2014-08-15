/********************************************************************
	created:  2014/07/04	12:33:41
	filename: ITTransaction.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__
#define __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__

#include <utils/SmartPtr.h>

namespace threadingUtils
{
	template<typename T>
	class ITWritableTransaction : public virtual T
	{
	public:

		//! Typ wskaünika do transakcji
		typedef utils::shared_ptr<T> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransaction() {}

		//! \return Transakcja
		virtual const TransactionPtr transaction() = 0;
	};


	template<typename T>
	class ITReadableTransaction : public virtual T
	{
	public:
		//! Typ wskaünika do sta≥ej transakcji
		typedef utils::shared_ptr<const T> TransactionConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransaction() {}

		//! \return Transakcja
		virtual const TransactionConstPtr transaction() const = 0;
	};

	template<typename T>
	class ITTransaction : public virtual T
	{
	public:
		//! Typ wskaünika do transakcji
		typedef utils::shared_ptr<T> TransactionPtr;
		//! Typ wskaünika do sta≥ej transakcji
		typedef utils::shared_ptr<const T> TransactionConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITTransaction() {}
		//! \return Transakcja
		virtual const TransactionPtr transaction() = 0;
		//! \return Transakcja
		virtual const TransactionConstPtr transaction() const = 0;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__