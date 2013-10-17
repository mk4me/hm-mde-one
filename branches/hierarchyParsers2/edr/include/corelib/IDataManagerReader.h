#ifndef HEADER_GUARD_CORE_IDATAMANAGERREADER_H__
#define HEADER_GUARD_CORE_IDATAMANAGERREADER_H__

#include <list>
#include <corelib/BaseDataTypes.h>
#include <corelib/ObjectWrapperCollection.h>
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    //! Interfejs obserwatora zmian
    template <class Changes>
    class IChangesObserver
    {
    public:

        virtual ~IChangesObserver() {}

        //! \param previousValue 
        //! \param currentVal 
        //! \param type 
        //! \param modyfication 
        virtual void observe(const Changes & changes) = 0;
    };


    class IDataManagerReaderOperations
	{
	public:

        //! Typ zmian danych w managerze
        enum ModificationType {
            ADD_OBJECT,		//! Dodajemy obiekt do DM
            REMOVE_OBJECT,	//! Usuwamy obiekt z DM
            UPDATE_OBJECT	//! Aktualizujemy obiekt w DM
        };

		virtual ~IDataManagerReaderOperations() {}

		//! \param objects [out] Wszystkie obiekty DM
		virtual void getObjects(ConstObjectsList & objects) const = 0;
		//! \param objects [out] Obiekty pasujące do zapytania
		//! \param type Typ obniektów o które pytamy
		//! \param exact Czy typ musi być zgodny czy moga to być tez typy pochodne
		//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
		virtual void getObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact) const = 0;
		//! \param objects [out] Obiekty zarządzane przez DM
		//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
		virtual void getObjects(ObjectWrapperCollection& objects) const = 0;
		//! \param object Obiekt który sprawdzamy pod kątem zarządzania przez DM
		//! \return Prawda jesli dany OW jest zarządzany przez DM
		virtual const bool isManaged(const ObjectWrapperConstPtr & object) const = 0;
		//! \param type Czy DM aktualnie zarządza jakimkolwiek obiektem danego typu
		//! \param exact Czy typ musi się zgadzać czy może być pochodny
		//! \return Prawda jesli obiekt danego typu jest zarządzany przez DM
		virtual const bool hasObject(const TypeInfo & type, bool exact) const = 0;
	};
	
	//! Podstawowe operacje związane z danymi - pobieranie danych, informacje o hierarchi typów danych, informacje o wspieranych typach danych
    class IDataManagerReader : public IDataManagerReaderOperations
    {
	public:
		//! Obiekt opisujący zmianę w DM
		struct ObjectChange
		{
			ObjectWrapperConstPtr previousValue;	//! Kopia OW przed zmianą, nie jest zarejestrowana w DM
			ObjectWrapperConstPtr currentValue;		//! Aktualny OW zarejestrowany w DM
			TypeInfo type;							//! Typ OW
			ModificationType modyfication;			//! Typ zmiany na OW
		};

		//! Agregat zmian w DM
		typedef std::list<ObjectChange> ChangeList;

		typedef IChangesObserver<ChangeList> IObjectObserver;
		//! Wskaźnik na obiek obserwujący zmiany
		typedef utils::shared_ptr<IObjectObserver> ObjectObserverPtr;
		typedef utils::shared_ptr<IDataManagerReaderOperations> TransactionPtr;

    public:

		virtual ~IDataManagerReader() {}

		//! Dodaje obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void addObserver(const ObjectObserverPtr & objectWatcher) = 0;
		//! Usuwa obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void removeObserver(const ObjectObserverPtr & objectWatcher) = 0;
		//! \return Transakcja odczytu danych
		virtual TransactionPtr transaction() const = 0;
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // HEADER_GUARD_CORE_IDATAMANAGERREADER_H__