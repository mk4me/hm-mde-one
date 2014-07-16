/********************************************************************
    created:  2013/01/17
    created:  17:1:2013   16:47
    filename: StreamDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___STREAMDATAMANAGER_H__
#define HEADER_GUARD___STREAMDATAMANAGER_H__

#include <map>
#include <list>
#include <threading/SynchronizationPolicies.h>
#include <corelib/IParserManagerReader.h>
#include <corelib/IStreamDataManager.h>
#include <corelib/IStreamManagerReader.h>

//TODO
//implementacja, wyci�gni�cie cz�ci wsp�lnej fileDM i streamDM -> parsery i wi�ksza cz�c logiki parsowania i inicjalizacji

namespace core {

	class StreamDataManager : public IStreamDataManager, public IStreamManagerReader
	{
	public:
		virtual const IStreamDataManager::TransactionPtr transaction()
		{
			return IStreamDataManager::TransactionPtr();
		}

		//! \data Dane wchodz�ce pod kontrol� DM
		virtual void addStream(const IStreamManagerReaderOperations::StreamData & stream)
		{

		}

		//! Dane usuwane z DM
		virtual void removeStream(const std::istream * stream)
		{

		}

		virtual const bool tryAddStream(const IStreamManagerReaderOperations::StreamData & stream)
		{
			return false;
		}

		virtual const bool tryRemoveStream(const std::istream * stream)
		{
			return false;
		}

		//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
		virtual void getStreams(StreamsList & streams) const
		{

		}

		//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
		//! \return Prawda je�li plik jest zarz�dzany przez ten DM
		virtual const bool isManaged(const std::istream * stream) const
		{
			return false;
		}

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const std::istream * stream, ConstVariantsList & objects) const
		{

		}

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const std::istream * stream, VariantsCollection & objects) const
		{

		}

		//! \param streamWatcher Rejestrowany obiekt obserwuj�cy zmiany managera strumieni
		virtual void addObserver(const StreamObserverPtr & streamWatcher)
		{

		}

		//! \param streamWatcher Wyrejestrowywany obiekt obserwuj�cy zmiany managera strumieni
		virtual void removeObserver(const StreamObserverPtr & streamWatcher)
		{

		}

		virtual IStreamManagerReader::TransactionPtr transaction() const
		{
			return IStreamManagerReader::TransactionPtr();
		}
	};
}

#endif	//	HEADER_GUARD___STREAMDATAMANAGER_H__
