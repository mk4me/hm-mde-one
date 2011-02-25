/********************************************************************
	created:  2011/02/11
	created:  11:2:2011   15:32
	filename: ObjectWrapper.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__OBJECTWRAPPER_H__
#define __HEADER_GUARD_CORE__OBJECTWRAPPER_H__

#include <list>
#include <string>
#include <typeinfo>
#include <boost/any.hpp>
#include <boost/type_traits.hpp>
#include <utils/PtrPolicyBoost.h>
#include <utils/PtrPolicyRaw.h>
#include <utils/PtrPolicyOSG.h>
#include <utils/Debug.h>
#include <core/SmartPtr.h>


////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Deklaracja typu. Trzeba go specjalizowaæ za pomoc¹ makr. Ta wersja bêdzie
//! rzucaæ statyczn¹ asercj¹.
template <class T>
class ObjectWrapperT : public ObjectWrapper
{
public:
    typedef T* Ptr;
    Ptr get()
    {
        UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego naglowka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
        return nullptr;
    }
    void set(Ptr)
    {
        UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego naglowka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
    }
};

//! Baza dla typu wrapuj¹cego jakiœ obiekt. Poza trzymaniem metadanych klasy pochodne
//! trzymaj¹ referencje do obiektów.
class ObjectWrapper
{
public:
    //! Struktura reprezentuj¹ca typ obiektu.
    struct Type
    {
        //! Wewnêtrzna informacja o typie.
        const std::type_info& typeinfo;

        //! \param typeinfo
        Type(const std::type_info& typeinfo) :
        typeinfo(typeinfo)
        {}

        //! \param type
        Type(const Type& type) :
        typeinfo(type.typeinfo)
        {}

        //! Jawny operator rzutowania na type_info.
        operator const std::type_info&() const
        {
            return typeinfo;
        }

        //! \param obj
        bool operator<(const Type& obj) const
        {
            return typeinfo.before(obj.typeinfo) != 0;
        } 
    };

protected:
    //! Nazwa instancji.
    std::string name;
    //! Nazwa typu.
    std::string className;
    //! Hash identyfikuj¹cy typ.
    std::size_t classID;

public:
    //! \param object
    //! \param name
    //! \return Wrapper obiektu.
    template <class T>
    static shared_ptr<ObjectWrapper> createWrapper( T* object = nullptr, const std::string& name = "")
    {
        typedef ObjectWrapperT<T> Wrapper;
        Wrapper * wrapper = new Wrapper();
        wrapper->set(Wrapper::Ptr(object));
        wrapper->setName(name);
        return shared_ptr<ObjectWrapper>(wrapper);
    }

    //! \return Nazwa dla zadanego typu.
    template <class T>
    static const char* getClassName(T* dummy = nullptr)
    {
        return ObjectWrapperT<T>::className();
    }

protected:
    //! \param className
    //! \param classId
    ObjectWrapper(const char* className, const std::type_info& type) :
    className(className), classID(type.hash_code())
    {}

public:
    //! Pusty polimorficzny destruktor.
    virtual ~ObjectWrapper() {}

public:
    //! \param dummy Pozostawiæ pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
    template <class T>
    auto get(bool exact = false, T* dummy = nullptr) -> decltype( reinterpret_cast<ObjectWrapperT<T>*>(nullptr)->get() )
    {   
        typedef ObjectWrapperT<T> Wrapper;
        Wrapper::Ptr result;
        if ( tryGet<T>(result) ) {
            return result;
        } else {
            throw std::runtime_error("Invalid wrapped type.");
        }
    }

    //! \param dummy Pozostawiæ pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
    template <class T>
    bool tryGet( typename ObjectWrapperT<T>::Ptr& target, bool exact = false )
    {
        typedef ObjectWrapperT<T> Wrapper;
        if ( exact ) {
            if ( areTypesEqual(getTypeInfo(), typeid(T)) ) {
                target = static_cast<Wrapper*>(this)->get();
                return true;
            }
        } else {
            Wrapper* wrapper = dynamic_cast<Wrapper*>(this);
            if ( wrapper ) {
                target = wrapper->get();
                return true;
            }
        }
        return false;
    }

    //! \param dummy Pozostawiæ pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
    template <class T>
    void set( const typename ObjectWrapperT<T>::Ptr& object )
    {   
        typedef ObjectWrapperT<T> Wrapper;
        Wrapper* wrapper = dynamic_cast<Wrapper*>(this);
        if ( !wrapper ) {
            throw std::runtime_error("Invalid wrapped type.");
        } else {
            return wrapper->set(object);
        }
    }

    //! \return ID typu.
    inline std::size_t getClassID() const
    { 
        return classID;
    }
    //! \return Nazwa typu.
    inline const std::string& getClassName() const
    {
        return className;
    }
    //! \return Nazwa obiektu.
    inline const std::string& getName() const
    {
        return name;
    }
    //! \param name Nazwa obiektu.
    inline void setName(const std::string& name)
    {
        this->name = name;
    }

    //! \param type Typ do porównania.
    //! \return Czy to te same typy?
    inline bool isTypeEqual(const std::type_info& type)
    {
        return areTypesEqual(getTypeInfo(), type);
    }

    //! \param type 
    //! \return Czy obiekt wspira okreœlony typ?
    virtual bool supports(const std::type_info& type) const = 0;

    //! \return Informacje o typie.
    virtual const std::type_info& getTypeInfo() const = 0;

    //! \param supported Lista wspieranych rozszerzeñ.
    virtual void appendSupported(std::list<Type>& supported) const = 0;

    //! \return Czy wrappowany obiekt jest wyzerowany?
    virtual bool isNull() const = 0;

protected:
    static inline bool areTypesEqual(const std::type_info& t1, const std::type_info& t2)
    {
        // tutaj u¿ywamy sta³ej z boost/any - pozwala okreœliæ, czy lepiej porównywaæ po nazwie, czy po adresie
#ifdef BOOST_AUX_ANY_TYPE_ID_NAME
        return std::strcmp(t1.name(), t2.name()) == 0;
#else
        return t1 == t2;
#endif
    }
};

typedef shared_ptr<ObjectWrapper> ObjectWrapperPtr;
typedef shared_ptr<const ObjectWrapper> ObjectWrapperConstPtr;
typedef weak_ptr<ObjectWrapper> ObjectWrapperWeakPtr;

//! Pomocniczy typ bazowy, zarz¹dzaj¹cy obiektem za pomoc¹ parametru
//! PtrPolicy. Tego typu nigdy nie u¿ywa siê wprost.
template <class T, class PtrPolicy>
class __ObjectWrapperT : public ObjectWrapper, protected PtrPolicy
{
    UTILS_STATIC_ASSERT( (utils::is_ptr_policy<PtrPolicy>::value), "Pointer policy is invalid. This will cause more errors." );
public:
    //!
    typedef PtrPolicy PtrPolicy;
    //!
    typedef PtrPolicy Base;
    //! Wrappowany obiekt.
    typedef typename Base::Ptr<T>::Type Ptr;

protected:
    //! Wrappowany obiekt.
    Ptr wrapped;

protected:
    //! \param wrapped
    __ObjectWrapperT(const char* className)
    : ObjectWrapper(className, typeid(T))
    {
        PtrPolicy::initPtr(wrapped);
    }
    //!
    virtual ~__ObjectWrapperT()
    {
        PtrPolicy::setPtr<T>(wrapped, nullptr);
    }

public:
    //! \return Wrappowany obiekt.
    const Ptr& get() const
    {
        return wrapped;
    }
    //! \return Wrappowany obiekt.
    Ptr get()
    {
        return wrapped;
    }
    //! \param wrapped Wrappowany obiekt.
    void set(const Ptr& wrapped)
    {
        PtrPolicy::setPtr(this->wrapped, wrapped);
    }
    //! \return Informacja o typie.
    virtual const std::type_info& getTypeInfo() const
    {
        return typeid(T);
    }
    //! \param type 
    //! \return Czy obiekt wspira okreœlony typ?
    virtual bool supports(const std::type_info& type) const
    {
        return ObjectWrapper::areTypesEqual(type, typeid(T));
    }
    //! \param supported
    virtual void appendSupported(std::list<ObjectWrapper::Type>& supported) const
    {
        supported.push_back(typeid(T));
    }
    //! \return Czy wewnêtrzny wskaŸnik jest wyzerowany?
    virtual bool isNull() const
    {
        return wrapped ? false : true;
    }
};

template <class T, class B>
class __ObjectWrapperT<T, ObjectWrapperT<B> > : public ObjectWrapperT<B>
{
    UTILS_STATIC_ASSERT( (boost::is_base_of<B, T>::value), "No inheritance." );
public:
    //! Wrappowany obiekt.
    typedef ObjectWrapperT<B> Base;
    //! U¿ycie PtrPolicy typu bazowego.
    typedef typename Base::PtrPolicy PtrPolicy;
    //! U¿ywany wskaŸnik.
    typedef typename PtrPolicy::Ptr<T>::Type Ptr;
public:
    //! \param wrapped
    __ObjectWrapperT(const char* className)
    : Base(className)
    {}
public:
    //! \return Wrappowany obiekt.
    const Ptr& get() const
    {
        return static_pointer_cast<T>(this->wrapped);
    }
    //! \return Wrappowany obiekt.
    Ptr get()
    {
        return static_pointer_cast<T>(this->wrapped);
    }
    //! \param wrapped Wrappowany obiekt.
    void set(const Ptr& wrapped)
    {
        Base::set(static_pointer_cast<B>(wrapped));
    }
    //! \return Informacja o typie.
    virtual const std::type_info& getTypeInfo() const
    {
        return typeid(T);
    }
    //! \param type 
    //! \return Czy obiekt wspira okreœlony typ?
    virtual bool supports(const std::type_info& type) const
    {
        if ( ObjectWrapper::areTypesEqual(type, typeid(T)) ) {
            return true;
        } else {
            return Base::supports(type);
        }
    }
    //! \param supported
    virtual void appendSupported(std::list<ObjectWrapper::Type>& supported) const
    {
        Base::appendSupported(supported);
        supported.push_back(typeid(T));
    }
};

template <class T> 
struct __ObjectWrapperBaseSelector
{
    typedef T Type;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

// Makro tworz¹ce specjalizacjê ObjectWrapperT. Musi wystêpowaæ w globalnym
// namespace. Drugim parametrem mo¿e byæ dowolny typ maj¹cy cechy ptrPolicy,
// wbudowane: PtrPolicyRaw, PtrPolicyBoost, PtrPolicyOSG
#define CORE_DEFINE_WRAPPER(type, ptrPolicy)                                \
namespace core {                                                            \
    template <>                                                             \
    class ObjectWrapperT<type> : public __ObjectWrapperT<type,              \
        __ObjectWrapperBaseSelector<ptrPolicy>::Type>                       \
    {                                                                       \
    public:                                                                 \
        static const char* className() { return #type; }                    \
    public:                                                                 \
        ObjectWrapperT(const char* className = #type) :                     \
        __ObjectWrapperT<type,                                              \
            __ObjectWrapperBaseSelector<ptrPolicy>::Type>(className)        \
        {}                                                                  \
    };                                                                      \
    template <>                                                             \
    struct __ObjectWrapperBaseSelector<type>                                \
    {                                                                       \
        typedef ObjectWrapperT<type> Type;                                  \
    };                                                                      \
}

#endif  // __HEADER_GUARD_CORE__OBJECTWRAPPER_H__