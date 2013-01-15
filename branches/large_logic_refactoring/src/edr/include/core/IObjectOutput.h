/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   15:39
    filename: IObjectOutput.h
    author:	  Piotr Gwiazdowski

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IOBJECTOUTPUT_H__
#define HEADER_GUARD_CORE__IOBJECTOUTPUT_H__

#include <boost/type_traits.hpp>
#include <core/SmartPtr.h>
#include <stdexcept>
#include <utils/Debug.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
#include <core/ILog.h>
#include <core/DataAccessors.h>

namespace plugin
{
    //! Interfejs zapewniający dostęp do zapisu danych wyjściowych z elementów przetwarzających, które będą przekazane nastepnikom
    class IObjectOutput
    {
    public:
        class OutputObjectsCollection;
        typedef core::shared_ptr<OutputObjectsCollection> OutputObjectsCollectionPtr;
        typedef core::shared_ptr<const OutputObjectsCollection> OutputObjectsCollectionConstPtr;
        typedef core::weak_ptr<OutputObjectsCollection> OutputObjectsCollectionWeakPtr;
        typedef core::weak_ptr<const OutputObjectsCollection> OutputObjectsCollectionConstWeakPtr;

        //! Klasa proxy przykrywająca możliwości ObjectWrapperCollection. Pozwala ładować dane do kolekcji w formie smart pointerów do typów domenowych lub ObjectWrapperPtr.
        class OutputObjectsCollection
        {
        public:

			OutputObjectsCollection & operator= (const OutputObjectsCollection & other)
			{
				if (this != &other) // protect against invalid self-assignment
				{
					collection = other.collection;
				}
				// by convention, always return *this
				return *this;
			}

            //! Konstruktor inicjujący proxy kolekcją object wrapperów
            OutputObjectsCollection(const core::ObjectWrapperCollectionPtr & collection = core::ObjectWrapperCollectionPtr())
                : collection(collection)
            {

            }

            //! Destruktor
            ~OutputObjectsCollection()
            {

            }

            //! \param object ObjectWrapperPtr z obiektem domenowym dodawany do kolekcji
            void addObject(const core::ObjectWrapperPtr & object)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");
                collection->push_back(object);
            }

            //! \param object ObjectWrapperPtr z obiektem domenowym dodawany do kolekcji
            void addObject(const core::ObjectWrapperConstPtr & object)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");
                collection->push_back(object);
            }

            //! Wzorzec metody addObject akceptujący smart pointery do typów domenowych. Wewnętrznie opakowuje dane w odpowiedni ObjectWrapperPtr i dodaje do kolekcji
            template<class T>
            void addObject(const T & object, const std::string & name, const std::string & source)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");

                ObjectWrapperPtr obj = __setObjectPointerResolver(object, name, source, boost::is_pointer<T>());

                collection->push_back(obj);
            }

        private:

            template <class T>
            core::ObjectWrapperPtr __setObjectPointerResolver(const T& object, boost::true_type)
            {
                // rev
                //UTILS_STATIC_ASSERT(false, "Do obiektów domenowych należy używać inteligentnych wskazników.");
                return core::ObjectWrapperPtr();
            }

            template <class T>
            core::ObjectWrapperPtr __setObjectPointerResolver(const T& object, const std::string & name, const std::string & source, boost::false_type)
            {
                return __setObjectPODResolver(object, name, source, boost::is_pod<T>());
            }

            template <class T>
            core::ObjectWrapperPtr __setObjectPODResolver(const T& object, const std::string & name, const std::string & source, boost::true_type)
            {
                // rev
                //UTILS_STATIC_ASSERT(false, "Niezaimplementowane");
                return core::ObjectWrapperPtr();
            }

            template <class SmartPtr>
            core::ObjectWrapperPtr __setObjectPODResolver(const SmartPtr& object, const std::string & name, const std::string & source, boost::false_type)
            {
                typedef typename SmartPtr::element_type Type;
                // jeżeli tutaj jest błąd oznacza to, że przekazany typ nie jest ani POD, ani inteligentnym wskaźnikiem.
                // rev
                //UTILS_STATIC_ASSERT(ObjectWrapperTraits<Type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
                //UTILS_STATIC_ASSERT((boost::is_same<SmartPtr, ObjectWrapperT<Type>::Ptr>::value), "SmartPtr nie odpowiada inteligetnemu wskaźnikowi odbsługującemu zadany typ");

                if(object == nullptr){
                    throw std::runtime_error("Could not create wprapper for nullptr");
                }

                core::ObjectWrapperPtr ret(core::ObjectWrapper::create<Type>());
                ret->set(object, name, source);
                return ret;
            }

        private:
            //! Kolekcja danych którą wypełniamy przez to proxy
            core::ObjectWrapperCollectionPtr collection;
        };

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectOutput() {}

        //! \return Liczba slotów wyjściowych.
        virtual int getNumOutputs() const = 0;

        //! Zwraca ObjectWrapper dla zadanego wejścia.
        virtual OutputObjectsCollection getObjects(int idx) = 0;
    };

    typedef core::shared_ptr<IObjectOutput> IObjectOutputPtr;

} // namespace core

#endif  // HEADER_GUARD_CORE__IOBJECTOUTPUT_H__
