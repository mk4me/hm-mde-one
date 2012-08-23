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

namespace core
{
    //! Interfejs zapewniaj�cy dost�p do zapisu danych wyj�ciowych z element�w przetwarzajacych, kt�re b�d� przekazane nastepnikom
    class IObjectOutput
    {
    public:
        class OutputObjectsCollection;
        typedef core::shared_ptr<OutputObjectsCollection> OutputObjectsCollectionPtr;
        typedef core::shared_ptr<const OutputObjectsCollection> OutputObjectsCollectionConstPtr;
        typedef core::weak_ptr<OutputObjectsCollection> OutputObjectsCollectionWeakPtr;
        typedef core::weak_ptr<const OutputObjectsCollection> OutputObjectsCollectionConstWeakPtr;

        //! Klasa proxy przykrywaj�ca mo�liwo�ci ObjectWrapperCollection. Pozwala �adowa� dane do kolekcji w formie smart pointer�w do typ�w domenowych lub ObjectWrapperPtr.
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

            //! Konstruktor inicjuj�cy proxy kolekcj� object wrapper�w
            OutputObjectsCollection(const ObjectWrapperCollectionPtr & collection = ObjectWrapperCollectionPtr())
                : collection(collection)
            {

            }

            //! Destruktor
            ~OutputObjectsCollection()
            {

            }

            //! \param object ObjectWrapperPtr z obiektem domenowym dodawany do kolekcji
            void addObject(const ObjectWrapperPtr & object)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");
                collection->addObject(object);
            }

            //! \param object ObjectWrapperPtr z obiektem domenowym dodawany do kolekcji
            void addObject(const ObjectWrapperConstPtr & object)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");
                collection->addObject(object);
            }

            //! Wzorzec metody addObject akceptuj�cy smart pointery do typ�w domenowych. Wewn�trznie opakowuje dane w odpowiedni ObjectWrapperPtr i dodaje do kolekcji
            template<class T>
            void addObject(const T & object, const std::string & name, const std::string & source)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");

                ObjectWrapperPtr obj = __setObjectPointerResolver(object, name, source, boost::is_pointer<T>());

                collection->addObject(obj);
            }

        private:

            template <class T>
            ObjectWrapperPtr __setObjectPointerResolver(const T& object, boost::true_type)
            {
                // rev
                //UTILS_STATIC_ASSERT(false, "Do obiekt�w domenowych nale�y u�ywa� inteligentnych wskaznikow.");
                return ObjectWrapperPtr();
            }

            template <class T>
            ObjectWrapperPtr __setObjectPointerResolver(const T& object, const std::string & name, const std::string & source, boost::false_type)
            {
                return __setObjectPODResolver(object, name, source, boost::is_pod<T>());
            }

            template <class T>
            ObjectWrapperPtr __setObjectPODResolver(const T& object, const std::string & name, const std::string & source, boost::true_type)
            {
                // rev
                //UTILS_STATIC_ASSERT(false, "Niezaimplementowane");
                return ObjectWrapperPtr();
            }

            template <class SmartPtr>
            ObjectWrapperPtr __setObjectPODResolver(const SmartPtr& object, const std::string & name, const std::string & source, boost::false_type)
            {
                typedef typename SmartPtr::element_type Type;
                // je�eli tutaj jest b��d oznacza to, �e przekazany typ nie jest ani POD, ani inteligentnym wska�nikiem.
                // rev
                //UTILS_STATIC_ASSERT(ObjectWrapperTraits<Type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
                //UTILS_STATIC_ASSERT((boost::is_same<SmartPtr, ObjectWrapperT<Type>::Ptr>::value), "SmartPtr nie odpowiada inteligetnemu wska�nikowi odbslugujacemu zadany typ");

                if(object == nullptr){
                    throw std::runtime_error("Could not create wprapper for nullptr");
                }

                ObjectWrapperPtr ret(core::ObjectWrapper::create<Type>());
                ret->set(object, name, source);
                return ret;
            }

        private:
            //! Kolekcja danych kt�r� wype�niamy przez to proxy
            ObjectWrapperCollectionPtr collection;
        };

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectOutput() {}

        //! \return Liczba slot�w wyj�ciowych.
        virtual int getNumOutputs() const = 0;

        //! Zwraca ObjectWrapper dla zadanego wej�cia.
        virtual OutputObjectsCollection getObjects(int idx) = 0;
    };

    typedef shared_ptr<IObjectOutput> IObjectOutputPtr;

} // namespace core

#endif  // HEADER_GUARD_CORE__IOBJECTOUTPUT_H__
