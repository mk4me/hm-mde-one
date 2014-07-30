/********************************************************************
	created:  2014/07/15	11:42:06
	filename: IShallowCopyFilterManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__ISHALLOWCOPYFILTERMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__ISHALLOWCOPYFILTERMANAGER_H__

#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>

namespace hmdbCommunication
{

	//! Manager filtrów p³ytkiej kopii bazy danych
	class IShallowCopyFilterManager
	{
	public:
		//! Destruktor
		virtual ~IShallowCopyFilterManager() {}

		//! \param filter Filtr danych który rejestrujemy
		//! \return Indeks zarejestrowanego filtra
		virtual const int registerFilter(ShallowCopyFilter * filter) = 0;

		//! \return Iloœæ zarejestrowanych filtrów
		virtual const int size() const = 0;

		//! \return Czy mamy zarejestrowane jakieœ filtry
		virtual const bool empty() const = 0;

		//! \param idx Indeks filtra
		//! \return Filtr o zadanym indeksie
		virtual const ShallowCopyFilterPtr dataFilter(int idx) const = 0;

		//! \param inShallow Wejœciowa p³ytka kopia do filtrowania
		//! \param outShallow Wyjœciowa, przefiltrowana p³ytka kopia
		//! \param filter Filtr u¿ywany do filtrowania
		virtual void filterShallowCopy(const ShallowCopy & inShallow,
			ShallowCopy & outShallow,
			const ShallowCopyFilter * filter) const = 0;
	};

}


#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__ISHALLOWCOPYFILTERMANAGER_H__