#ifndef HEADER_GUARD_CORE_IDATAMANAGERREADER_H__
#define HEADER_GUARD_CORE_IDATAMANAGERREADER_H__

#include <list>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
////////////////////////////////////////////////////////////////////////////////
namespace plugin {
////////////////////////////////////////////////////////////////////////////////

    //! Podstawowe operacje związane z danymi - pobieranie danych, informacje o hierarchi typów danych, informacje o wspieranych typach danych
    class IDataManagerReader
    {
	public:
		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_OBJECT,		//! Dodajemy obiekt do DM
			REMOVE_OBJECT,	//! Usuwamy obiekt z DM
			UPDATE_OBJECT	//! Aktualizujemy obiekt w DM
		};

		//! Obiekt opisujący zmianę w DM
		struct ObjectChange
		{
			core::ObjectWrapperConstPtr previousValue;	//! Kopia OW przed zmianą, nie jest zarejestrowana w DM
			core::ObjectWrapperConstPtr currentVal;		//! Aktualny OW zarejestrowany w DM
			core::TypeInfo type;							//! Typ OW
			ModificationType modyfication;			//! Typ zmiany na OW
		};

		//! Agregat zmian w DM
		typedef std::list<ObjectChange> ChangeList;

		//! Interfejs obserwatora zmian
		class IObjectObserver
		{
		public:
			//! \param previousValue 
			//! \param currentVal 
			//! \param type 
			//! \param modyfication 
			virtual void observe(const ChangeList & changes) = 0;
		};

		//! Wskaźnik na obiek obserwujący zmiany
		typedef core::shared_ptr<IObjectObserver> ObjectObserverPtr;

    public:
		//! Dodaje obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void addObserver(const ObjectObserverPtr & objectWatcher) = 0;
		//! Usuwa obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void removeObserver(const ObjectObserverPtr & objectWatcher) = 0;

		//! \param objects [out] Obiekty pasujące do zapytania
		//! \param type Typ obniektów o które pytamy
		//! \param exact Czy typ musi być zgodny czy moga to być tez typy pochodne
		//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
        virtual void getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact = true) const = 0;
		//! \param objects [out] Obiekty zarządzane przez DM
		//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
        virtual void getObjects(core::ObjectWrapperCollection& objects) const = 0;
		//! \param object Obiekt który sprawdzamy pod kątem zarządzania przez DM
		//! \return Prawda jesli dany OW jest zarządzany przez DM
		virtual const bool isManaged(const core::ObjectWrapperConstPtr & object) const = 0;
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // HEADER_GUARD_CORE_IDATAMANAGERREADER_H__
