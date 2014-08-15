/********************************************************************
	created:  2014/07/07	12:33:04
	filename: HMDBLocalContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBLOCALCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBLOCALCONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IMemoryDataManager.h>
#include <corelib/IStreamDataManager.h>
//#include <corelib/IHierarchyItem.h>

namespace hmdbCommunication
{

	class HMDBLocalContext : public IHMDBLocalContext, public core::IDataManagerReader::IObjectObserver
	{
	private:

		class HMDBLocalContextTransaction;
		friend class HMDBLocalContextTransaction;

	public:

		//! \param dataContext Kontekst dancyh z dostepem do storage
		//! \param mdm MemoryManager
		//! \param sdm StreaManager
		HMDBLocalContext(IHMDBDataContextPtr dataContext, core::IMemoryDataManager * mdm,
			core::IStreamDataManager * sdm);

		//! Destruktor wirtualny
		virtual ~HMDBLocalContext();

		virtual void observe(const core::IDataManagerReader::ChangeList & changes);

	//! ------------ Opis kontekstu ---------------------------

		//! \return Kontekst danych
		virtual const IHMDBDataContextPtr dataContext();
		//! \return Kontekst danych
		virtual const IHMDBDataContextConstPtr dataContext() const;

	//! ------------- Operacje --------------------------------

		//! \return Czy dane o które pytamy pochodza z tego Ÿród³a
		virtual const bool isMyData(core::VariantConstPtr data) const;
		//! \param fileName Nazwa pliku z p³ytkiej kopii bazy danych
		//! \return Za³adanowane dane z pliku
		virtual const core::ConstVariantsList data(const std::string & fileName) const;

		//! \param fileName Plik jaki bêdê ³adowaæ ze storage do StreamManagera
		//! \return Czy uda³o siê za³adowac plik
		virtual const bool load(const std::string & fileName);
		//! \param fileName Plik jaki bêdê wy³adowywaæ ze StreamManagera
		//! \return Czy uda³o siê wy³adowac plik
		virtual const bool unload(const std::string & fileName);
		//! \param fileName Plik o który pytam czy jest za³¹dowany
		//! \return Czy plik jest za³adowany
		virtual const bool isLoaded(const std::string & fileName) const;

		//! \param data Dane jakie bêdê ³adowa³ do MemoryManagera
		//! \return Czy uda³o siê za³adowac dane
		virtual const bool load(const core::VariantPtr data);
		//! \param data Dane jakie bêdê wy³adowa³ z MemoryManagera
		//! \return Czy uda³o siê wy³adowac dane
		virtual const bool unload(const core::VariantConstPtr data);

		virtual const TransactionPtr transaction();

		virtual const TransactionConstPtr transaction() const;

	private:		

		const core::ConstVariantsList rawData(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction) const;

		const bool rawLoad(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction);
		const bool rawUnload(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction);

		const bool rawLoad(const core::VariantPtr data, const core::IMemoryDataManager::TransactionPtr memoryTransaction);
		const bool rawUnload(const core::VariantConstPtr data, const core::IMemoryDataManager::TransactionPtr memoryTransaction);

		const bool rawIsMyData(core::VariantConstPtr data) const;

		const bool rawIsLoaded(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction) const;

	private:
		//! kontekst danych
		IHMDBDataContextPtr dataContext_;
		//! Manager danych
		core::IMemoryDataManager * mdm;
		//! Manager strumieni
		core::IStreamDataManager * sdm;
		//! Zbiór danych którym zarz¹dzam
		std::list<core::VariantConstPtr> myData_;
	};

}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBLOCALCONTEXT_H__