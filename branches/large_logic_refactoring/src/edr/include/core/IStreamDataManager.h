/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:04
    filename: IStreamDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMDATAMANAGER_H__
#define HEADER_GUARD___ISTREAMDATAMANAGER_H__

#include <core/SmartPtr.h>
#include <core/IMemoryDataManager.h>

namespace std {

	class istream;

}

namespace plugin {

	//! Zbi�r operacji na strumieniach obs�ugiwanych przez aplikacj�
	class IStreamDataManagerOperations
	{
	public:
		//! \data Dane wchodz�ce pod kontrol� DM
		virtual void addStream(std::istream * stream) = 0;
		//! Dane usuwane z DM
		virtual void removeStream(const std::istream * stream) = 0;

		const bool tryAddData(std::istream * stream)
		{
			bool ret = true;
			try{
				addStream(stream);
			}catch(...){
				ret = false;
			}

			return ret;
		}

		const bool tryRemoveStream(const std::istream * stream)
		{
			bool ret = true;
			try{
				removeStream(stream);
			}catch(...){
				ret = false;
			}

			return ret;
		}
	};

	//! Manager udost�pniaj�cy operacje na strumieniach zarz�dzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
	class IStreamDataManager : public IStreamDataManagerOperations
	{
	public:
		//! Typ transkacji na strumieniach - dzia�a w oparciu o RAII -> pr�buje "commitowa�" zmiany przy niszczeniu obiektu transakcji
		typedef core::shared_ptr<IStreamDataManagerOperations> TransactionPtr;

	public:
		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction(const IMemoryDataManager::TransactionPtr & memoryTransaction) = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
