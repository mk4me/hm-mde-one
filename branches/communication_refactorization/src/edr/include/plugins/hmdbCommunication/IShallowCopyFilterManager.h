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

	//! Manager filtr�w p�ytkiej kopii bazy danych
	class IShallowCopyFilterManager
	{
	public:
		//! Destruktor
		virtual ~IShallowCopyFilterManager() {}

		//! \param filter Filtr danych kt�ry rejestrujemy
		//! \return Indeks zarejestrowanego filtra
		virtual const int registerFilter(ShallowCopyFilter * filter) = 0;

		//! \return Ilo�� zarejestrowanych filtr�w
		virtual const int size() const = 0;

		//! \return Czy mamy zarejestrowane jakie� filtry
		virtual const bool empty() const = 0;

		//! \param idx Indeks filtra
		//! \return Filtr o zadanym indeksie
		virtual const ShallowCopyFilterPtr dataFilter(int idx) const = 0;

		//! \param inShallow Wej�ciowa p�ytka kopia do filtrowania
		//! \param outShallow Wyj�ciowa, przefiltrowana p�ytka kopia
		//! \param filter Filtr u�ywany do filtrowania
		virtual void filterShallowCopy(const ShallowCopy & inShallow,
			ShallowCopy & outShallow,
			const ShallowCopyFilter * filter) const = 0;
	};

}


#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__ISHALLOWCOPYFILTERMANAGER_H__