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
#include <core/Log.h>

namespace core 
{
    class IObjectOutput
    {
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectOutput() {}

        //! \return Liczba slot�w wyj�ciowych.
        virtual int getNumObjects() const = 0;

        //! Zwraca OW dla zadanego wej�cia.
        virtual ObjectWrapperPtr getWrapper(int idx) = 0;

    public:
        //! Wy�uskanie wska�nika na obiekt domenowy ze �r�d�a przy za�o�eniu jego zmienno�ci.
        template <class T>
        void setObject(int outputNo, const T& object)
        {
            __setObjectPointerResolver(outputNo, object, boost::is_pointer<T>());
        }

    private:

        template <class T>
        void __setObjectPointerResolver(int outputNo, const T& object, boost::true_type)
        {
            UTILS_STATIC_ASSERT(false, "Do obiektow domenowych nalezy uzywac inteligentnych wskaznikow.");
        }

        template <class T>
        void __setObjectPointerResolver(int outputNo, const T& object, boost::false_type)
        {
            __setObjectPODResolver(outputNo, object, boost::is_pod<T>());
        }

        template <class T>
        void __setObjectPODResolver(int outputNo, const T& object, boost::true_type)
        {
            UTILS_STATIC_ASSERT(false, "Niezaimplementowane");
        }

        template <class SmartPtr>
        void __setObjectPODResolver(int outputNo, const SmartPtr& object, boost::false_type)
        {
            // je�eli tutaj jest b��d oznacza to, �e przekazany typ nie jest ani POD, ani inteligentnym wska�nikiem.
            typedef typename SmartPtr::element_type Type;
            auto wrapper = getWrapper(outputNo);
            wrapper->set(object);
        }

    };

    typedef shared_ptr<IObjectOutput> IObjectOutputPtr;

} // namespace core

#endif  // HEADER_GUARD_CORE__IOBJECTOUTPUT_H__