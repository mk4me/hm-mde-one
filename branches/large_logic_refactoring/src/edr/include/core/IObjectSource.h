/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:38
    filename: IObjectSource.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IOBJECTSOURCE_H__
#define HEADER_GUARD_CORE__IOBJECTSOURCE_H__

#include <stdexcept>
#include <utils/Debug.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
#include <core/ILog.h>

namespace plugin 
{
    //! Interfejs dający dostęp do danych wejsciowych. Nie ma gwarancji że dane są dostępne i zainicjalizowane - należe to każdorazowo sprawdzać.
    class IObjectSource
    {
    public:

        class InputObjectsCollection;
        typedef core::shared_ptr<InputObjectsCollection> InputObjectsCollectionPtr;
        typedef core::shared_ptr<const InputObjectsCollection> InputObjectsCollectionConstPtr;
        typedef core::weak_ptr<InputObjectsCollection> InputObjectsCollectionWeakPtr;
        typedef core::weak_ptr<const InputObjectsCollection> InputObjectsCollectionConstWeakPtr;

        //! Klasa proxy przykrywająca możliwość przechowywania ObjecWrapperów przez ObjecWrapperCollection. Umożliwia jedynie pobieranie niemodyfikowalnych danych z kolekcji.
        //! Dane pobierane są w rozpakowanej już formie do rzeczywistych typów (wyciągnięte z ObjectWrapperów). Są to smart pointery na const obiekty.
        class InputObjectsCollection
        {
        public:
            //! Return type resolver - poprzez operator konwersji pozwala kompilatorowi rozpoznać typ zwracany i wykonać dla niego konwersję
            struct get_t
            {
            public:
                //! const ObjectWrapperPtr któy będziemy ropzakowywać
                const core::ObjectWrapperConstPtr & constObjectWrapperPtr;
                //! Czy typ musi być dokładnie taki sam jaki przechowuje ObjectWraper czy może to być jakiś typ niżej w hierarchi dziedziczenia
                bool exact;

                //! Konstruktor inicjujący RtR
                get_t(const core::ObjectWrapperConstPtr & constObjectWrapperPtr, bool exact)
                    : constObjectWrapperPtr(constObjectWrapperPtr), exact(exact) {}

                //! Operator konwersji w formie wzorca robiący cała magię
                template<class SmartConstPtr>
                inline operator SmartConstPtr() const
                {
                    SmartConstPtr ret;
                    constObjectWrapperPtr->tryGet(ret, exact);
                    return ret;
                }

				//! Operator konwersji w formie wzorca robiący cała magię
				inline operator core::ObjectWrapperConstPtr() const
				{
					return constObjectWrapperPtr;
				}
            };

        public:

            //! Konstruktor inicjujący proxy kolekcją object wrapperów
            InputObjectsCollection(const core::ObjectWrapperCollectionConstPtr & collection = core::ObjectWrapperCollectionConstPtr())
                : collection(collection)
            {

            }

            //! Destruktor
            ~InputObjectsCollection()
            {

            }

            //! Zwraca dane w zadanej formie - RtR
            //! \param idx Indeks danych
            //! \return Return Type Resolver
            const get_t getObject(int idx) const
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");

                if(collection == nullptr){
                    throw std::runtime_error("Call beyond collection range or to nullptr");
                }

				auto it = collection->begin();
				std::advance(it, idx);

                const get_t ret(*it, false);
                return ret;
            }

            //! \return Czy kolekcja pusta (lub niezainicjowana - tak też się może zdażyć dla wejscia
            bool empty() const
            {
                if(collection == nullptr){
                    return true;
                }

                return collection->empty();
            }

            //! \return Rozmiar kolekcji danych - 0 jeśli pusta lub niezainicjowana
            int size() const
            {
                if(empty() == true){
                    return 0;
                }

                return collection->size();
            }


        private:
            //! Kolekcja danych którą przykrywamy tym proxy
            core::ObjectWrapperCollectionConstPtr collection;
        };

    // interfejs
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectSource() {}

        //! \return Liczba źródeł danych.
        virtual int getNumSources() const = 0;

        //! Wyłuskanie wskaźnika na obiekt domenowy ze źródła przy założeniu jego niezmienności.
        virtual InputObjectsCollection getObjects(int idx) const = 0;
    };

    typedef core::shared_ptr<IObjectSource> IObjectSourcePtr;
    
} // namespace core


#endif  // HEADER_GUARD_CORE__IOBJECTSOURCE_H__
