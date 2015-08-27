/********************************************************************
	created:  2014/07/04	12:33:41
	filename: ITTransaction.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__
#define __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__

#include <utils/SmartPtr.h>
#include <mutex>

namespace threadingUtils
{
	//! \tparam T Typ dla kt�rego realizujemy transakcj�
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITWritableTransaction
	{
	public:

		typedef T transaction_type;

		//! Typ wska�nika do transakcji
		typedef SmartPtr<T> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransaction() {}

		//! \return Transakcja
		virtual TransactionPtr transaction() = 0;
	};

	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITWritableTransactionObject : public virtual T, public ITWritableTransaction < T, SmartPtr>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransactionObject() {}
	};

	//! \tparam T Typ dla kt�rego realizujemy transakcj�
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITReadableTransaction
	{
	public:

		typedef T transaction_type;

		//! Typ wska�nika do sta�ej transakcji
		typedef SmartPtr<const T> TransactionConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransaction() {}

		//! \return Transakcja
		virtual TransactionConstPtr transaction() const = 0;
	};

	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITReadableTransactionObject : public virtual T, public ITReadableTransaction < T, SmartPtr >
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransactionObject() {}
	};

	//! \tparam T Typ dla kt�rego realizujemy transakcj�
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITTransaction : public ITWritableTransaction<T, SmartPtr>, public ITReadableTransaction<T, SmartPtr>
	{
	public:

		typedef T transaction_type;

	public:
		//! Destruktor wirtualny
		virtual ~ITTransaction() {}

		using ITWritableTransaction<T, SmartPtr>::transaction;
		using ITReadableTransaction<T, SmartPtr>::transaction;
	};

	//! \tparam T Typ dla kt�rego realizujemy transakcj�
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITTransactionObject : public virtual T, public ITTransaction<T, SmartPtr>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITTransactionObject() {}
	};

	//! Forward declaration
	template<typename TransactionT, typename SharedStateT>
	class TransactionImplHelper;

	//! Struktura wspoldzielonego stanu przez transakcje - obejmuje wewn�trzny
	//! stan transakcji oraz obiekt synchronizuj�cy
	//! \tparam SharedStateT Wsp�ldzielony stan kontekstu transakcji
	//! \tparam MutexType Typ obiektu synchronizuj�cego
	template<typename SharedStateT, class MutexType = std::recursive_mutex>
	struct TransactionSharedState
	{
		template<typename TransactionT, typename SharedStateT>
		friend class TransactionImplHelper;

	private:
		//! Typ mutexa
		typedef MutexType mutex_type;

	public:
		//! Typ wewnetrznego stanu
		typedef SharedStateT inner_shared_state_type;

	public:
		//! Wewn�trzny stan
		SharedStateT innerState;

	private:
		//! Obiekt synchronizuj�cy
		MutexType sync;
	};

	//! Klasa pomocnicza przy implementacji transakcji
	//! automatycznie zak�ada i �ci�ga blokad�
	//! \tparam TransactionT Typ transakcji
	//! \tparam SharedStateT Typ wsp�dzielonego stanu
	template<typename TransactionT, typename SharedStateT>
	class TransactionImplHelper : public TransactionT
	{
	public:

		typedef TransactionImplHelper<TransactionT, SharedStateT> ImplType;
		typedef utils::shared_ptr<SharedStateT> SharedStatePtr;

	protected:

		TransactionImplHelper(SharedStatePtr sharedState)
			: sharedState_(sharedState), lock(sharedState_->sync) {}
		virtual ~TransactionImplHelper() {}

		inline typename SharedStateT::inner_shared_state_type & sharedState()
		{
			return sharedState_->innerState;
		}

		inline const typename SharedStateT::inner_shared_state_type & sharedState() const
		{
			return sharedState_->innerState;
		}

	protected:
		SharedStatePtr sharedState_;
	private:
		std::lock_guard<typename SharedStateT::mutex_type> lock;
	};
}

#define TRANSACTION(obj) obj->transaction()
#define TRANSACTION_PART __t
#define BEGIN_TRANSACTION(obj) { auto TRANSACTION_PART = TRANSACTION(obj);
#define END_TRANSACTION }

#endif	// __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__