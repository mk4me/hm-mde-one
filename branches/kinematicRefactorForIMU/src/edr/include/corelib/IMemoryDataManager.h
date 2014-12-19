/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   20:08
    filename: IMemoryDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IMEMORYDATAMANAGER_H__
#define HEADER_GUARD___IMEMORYDATAMANAGER_H__

#include <corelib/ITransaction.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/IHierarchyItem.h>

namespace core {

	class IMemoryDataManagerBasicOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMemoryDataManagerBasicOperations() {}

		//! \data Dane wchodz¹ce pod kontrolê DM
		//! rzuca wyj¹tkiem jeœli nie uda siê dodaæ danych
		virtual void addData(const VariantPtr & data) = 0;
		//! Dane usuwane z DM
		//! rzuca wyj¹tkiem jeœli nie uda siê usun¹æ danych
		virtual void removeData(const VariantConstPtr & data) = 0;

		//! \param data Dane do dodaniwa w DM
		//! \return Czy uda³o siê poprawnie dodaæ dane do DM
		virtual const bool tryAddData(const VariantPtr & data) = 0;
		//! \param data Dane do dodaniwa w DM
		//! \return Czy uda³o siê poprawnie usun¹æ dane do DM
		virtual const bool tryRemoveData(const VariantConstPtr & data) = 0;
	};

    //! Operacje zwi¹zane z zarz¹dzaniem hierarchicznymi danymi
    class IMemoryDataManagerHierarchyOperations
    {
    public:
		//! Destruktor wirtualny
        virtual ~IMemoryDataManagerHierarchyOperations() {}
    
        //! Dodanie roota, odczytanie wszystkich danych w hierarchii i dodanie ich do DM
        //! \param ptr dodawany element
        virtual void addRoot(IHierarchyItemConstPtr ptr) = 0;
        //! Usuniêcie roota z DM
        //! \param ptr element do usuniêcia, musi byæ wczeœniej dodany
        virtual void removeRoot(IHierarchyItemConstPtr ptr) = 0;
        //! Wywo³ane, gdy hierarchia, lub jej dane siê zmieni³y
        //! \param ptr update'owany element
        virtual void updateRoot(IHierarchyItemConstPtr ptr) = 0;
    };

    class IMemoryDataManagerHierarchy : public IMemoryDataManagerHierarchyOperations
    {
    public:
        typedef IDataManagerReader::ModificationType ModificationType;
        //! Obiekt opisuj¹cy zmianê w DM
        struct HierarchyChange
        {
            IHierarchyItemConstPtr value;
            ModificationType modification;			//! Typ zmiany na hierarchii
        };

        typedef std::set<IHierarchyItemConstPtr>::const_iterator hierarchyConstIterator;  
        //! Agregat zmian w DM
        typedef std::list<HierarchyChange> HierarchyChangeList;
        typedef IChangesObserver<HierarchyChangeList> IHierarchyObserver;
        //! WskaŸnik na obiek obserwuj¹cy zmiany
        typedef utils::shared_ptr<IHierarchyObserver> HierarchyObserverPtr;
        typedef utils::shared_ptr<IDataManagerReaderOperations> TransactionPtr;

    public:
        virtual ~IMemoryDataManagerHierarchy() {}
        //! \param objectWatcher Obserwator DM do zarejestrowania
        virtual void addObserver(const HierarchyObserverPtr & objectWatcher) = 0;
        //! \param objectWatcher Obserwator DM do wyrejestrowania
        virtual void removeObserver(const HierarchyObserverPtr & objectWatcher) = 0;

        virtual hierarchyConstIterator hierarchyBegin() const = 0;
        virtual hierarchyConstIterator hierarchyEnd() const = 0;
    };

	//! Operacje zwi¹zane z zarz¹dzaniem danymi domenowymi aplikacji
	class IMemoryDataManagerOperations : public IMemoryDataManagerBasicOperations
	{
	public:

		virtual ~IMemoryDataManagerOperations() {}
		
		//! \param data Aktualizowane dane
		//! \param newData Nowa wartoœæ danych
		virtual void updateData(const VariantConstPtr & data, const VariantConstPtr & newData) = 0;

		virtual const bool tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData) = 0;
	};

    

	//! Interfejs do danych domenowych i zarz¹dzania nimi w aplikacji
	class IMemoryDataManager : public IMemoryDataManagerOperations
	{
	public:

		class IMemoryDataTransaction : public ITransaction, public IMemoryDataManagerOperations, public IDataManagerReaderOperations
		{
		public:
			virtual ~IMemoryDataTransaction() {}
		};

        class IMemoryDataHierarchyTransaction : public ITransaction, public IMemoryDataManagerHierarchyOperations
        {
        public:
            virtual ~IMemoryDataHierarchyTransaction() {}
        };

		//! Typ transakcji na danych domenowych - dzia³a w oparciu o RAII -> próbuje "commitowaæ" zmiany przy koñcu trwania ¿ycia obiektu transakcji
		typedef utils::shared_ptr<IMemoryDataTransaction> TransactionPtr;
        //! Typ transakcji dodaj¹cej dane w postaci hierarchicznej
        typedef utils::shared_ptr<IMemoryDataHierarchyTransaction> HierarchyTransactionPtr;

	public:

		virtual ~IMemoryDataManager() {}

		//! \return Nowa transakcja danych domenowych
		virtual TransactionPtr transaction() = 0;
        virtual HierarchyTransactionPtr hierarchyTransaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IMEMORYDATAMANAGER_H__
