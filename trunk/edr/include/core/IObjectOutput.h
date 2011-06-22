/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   15:39
    filename: IObjectOutput.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IOBJECTOUTPUT_H__
#define HEADER_GUARD_CORE__IOBJECTOUTPUT_H__

#include <stdexcept>
#include <boost/type_traits.hpp>
#include <utils/Debug.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
#include <core/Log.h>

namespace core 
{
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

            //! Konstruktor inicjuj�cy proxy kolekcj� object wrapper�w
            OutputObjectsCollection(const ObjectWrapperCollectionPtr & collection)
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

                if(collection == nullptr){
                    throw std::runtime_error("Call beyond collection range or to nullptr");
                }

                return collection->addObject(object);
            }

            //! \param object ObjectWrapperPtr z obiektem domenowym dodawany do kolekcji
            void addObject(const ObjectWrapperConstPtr & object)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");

                if(collection == nullptr){
                    throw std::runtime_error("Call beyond collection range or to nullptr");
                }

                return collection->addObject(object);
            }

            //! Wzorzec metody addObject akceptuj�cy smart pointery do typ�w domenowych. Wewn�trznie opakowuje dane w odpowiedni ObjectWrapperPtr i dodaje do kolekcji
            template<class T>
            void addObject(const T & object)
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");

                if(collection == nullptr){
                    throw std::runtime_error("Call beyond collection range or to nullptr");
                }

                ObjectWrapperPtr obj = __setObjectPointerResolver(object, boost::is_pointer<T>());

                return collection->addObject(obj);
            }

        private:

            template <class T>
            ObjectWrapperPtr __setObjectPointerResolver(const T& object, boost::true_type)
            {
                UTILS_STATIC_ASSERT(false, "Do obiektow domenowych nalezy uzywac inteligentnych wskaznikow.");
                return ObjectWrapperPtr();
            }

            template <class T>
            ObjectWrapperPtr __setObjectPointerResolver(const T& object, boost::false_type)
            {
                return __setObjectPODResolver(object, boost::is_pod<T>());
            }

            template <class T>
            ObjectWrapperPtr __setObjectPODResolver(const T& object, boost::true_type)
            {
                UTILS_STATIC_ASSERT(false, "Niezaimplementowane");
                return ObjectWrapperPtr();
            }

            template <class SmartPtr>
            ObjectWrapperPtr __setObjectPODResolver(const SmartPtr& object, boost::false_type)
            {
                // je�eli tutaj jest b��d oznacza to, �e przekazany typ nie jest ani POD, ani inteligentnym wska�nikiem.
                typedef typename SmartPtr::element_type Type;
                //auto wrapper = ObjectWrapper::create<Type>();
                auto wrapper = ObjectWrapper::create<SmartPtr::element_type>();
                wrapper->set(object);
                return wrapper;
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

        //! Zwraca OW dla zadanego wej�cia.
        virtual OutputObjectsCollection getObjects(int idx) = 0;

    //public:
    //    //! Wy�uskanie wska�nika na obiekt domenowy ze �r�d�a przy za�o�eniu jego zmienno�ci.
    //    template <class T>
    //    void setObject(int outputNo, const T& object)
    //    {
    //        __setObjectPointerResolver(outputNo, object, boost::is_pointer<T>());
    //    }

    //private:

    //    template <class T>
    //    void __setObjectPointerResolver(int outputNo, const T& object, boost::true_type)
    //    {
    //        UTILS_STATIC_ASSERT(false, "Do obiektow domenowych nalezy uzywac inteligentnych wskaznikow.");
    //    }

    //    template <class T>
    //    void __setObjectPointerResolver(int outputNo, const T& object, boost::false_type)
    //    {
    //        __setObjectPODResolver(outputNo, object, boost::is_pod<T>());
    //    }

    //    template <class T>
    //    void __setObjectPODResolver(int outputNo, const T& object, boost::true_type)
    //    {
    //        UTILS_STATIC_ASSERT(false, "Niezaimplementowane");
    //    }

    //    template <class SmartPtr>
    //    void __setObjectPODResolver(int outputNo, const SmartPtr& object, boost::false_type)
    //    {
    //        // je�eli tutaj jest b��d oznacza to, �e przekazany typ nie jest ani POD, ani inteligentnym wska�nikiem.
    //        typedef typename SmartPtr::element_type Type;
    //        auto wrapper = getWrapper(outputNo);
    //        wrapper->set(object);
    //    }

    };

    typedef shared_ptr<IObjectOutput> IObjectOutputPtr;

} // namespace core

#endif  // HEADER_GUARD_CORE__IOBJECTOUTPUT_H__