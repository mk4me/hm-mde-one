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
	//! \tparam T Typ fla którego realizujemy transakcjê
	template<typename T>
	class ITWritableTransaction
	{
	public:
		//! Typ wskaŸnika do transakcji
		typedef utils::shared_ptr<T> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransaction() {}

		//! \return Transakcja
		virtual const TransactionPtr transaction() = 0;
	};

	template<typename T>
	class ITWritableTransactionObject : public T, public ITWritableTransaction<T>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransactionObject() {}
	};

	//! \tparam T Typ fla którego realizujemy transakcjê
	template<typename T>
	class ITReadableTransaction
	{
	public:
		//! Typ wskaŸnika do sta³ej transakcji
		typedef utils::shared_ptr<const T> TransactionConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransaction() {}

		//! \return Transakcja
		virtual const TransactionConstPtr transaction() const = 0;
	};

	template<typename T>
	class ITReadableTransactionObject : public T, public ITReadableTransaction < T >
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransactionObject() {}
	};

	//! \tparam T Typ fla którego realizujemy transakcjê
	template<typename T>
	class ITTransaction : public ITWritableTransaction<T>, public ITReadableTransaction<T>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITTransaction() {}

		using ITWritableTransaction<T>::transaction;
		using ITReadableTransaction<T>::transaction;
	};

	//! \tparam T Typ fla którego realizujemy transakcjê
	template<typename T>
	class ITTransactionObject : public T, public ITTransaction<T>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITTransactionObject() {}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__