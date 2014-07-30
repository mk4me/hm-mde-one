/********************************************************************
    created:  2012/02/16
    created:  16:2:2012   13:11
    filename: ShallowCopyFilterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYFILTERMANAGER_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYFILTERMANAGER_H__

#include <plugins/hmdbCommunication/IShallowCopyFilterManager.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <vector>
#include <threadingUtils/SynchronizationPolicies.h>

namespace hmdbCommunication
{

	//! Manager filtrów płytkiej kopii bazy danych
	class ShallowCopyFilterManager : public IShallowCopyFilterManager
	{
	public:
		//! Domyślny konstruktor
		ShallowCopyFilterManager();
		//! Destruktor
		virtual ~ShallowCopyFilterManager();

		//! \param filter Filtr danych który rejestrujemy
		//! \return Indeks zarejestrowanego filtra
		virtual const int registerFilter(ShallowCopyFilter * filter);

		//! \return Ilość zarejestrowanych filtrów
		virtual const int size() const;

		//! \return Czy mamy zarejestrowane jakieś filtry
		virtual const bool empty() const;

		//! \param idx Indeks filtra
		//! \return Filtr o zadanym indeksie
		virtual const ShallowCopyFilterPtr dataFilter(int idx) const;

		//! \param inShallow Wejściowa płytka kopia do filtrowania
		//! \param outShallow Wyjściowa, przefiltrowana płytka kopia
		//! \param filter Filtr używany do filtrowania
		virtual void filterShallowCopy(const ShallowCopy & inShallow,
			ShallowCopy & outShallow,
			const ShallowCopyFilter * filter) const;

	private:
		//! Zarejestrowane filtry
		std::vector<ShallowCopyFilterPtr> filters;
		//! Obiekt synchronizujący
		mutable threadingUtils::RecursiveSyncPolicy sync;
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYFILTERMANAGER_H__
