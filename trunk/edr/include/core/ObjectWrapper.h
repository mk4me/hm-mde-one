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
#include <boost/type_traits.hpp>
#include <core/TypeInfo.h>
#include <utils/Debug.h>
#include <core/SmartPtr.h>
#include <utils/PtrPolicyBoost.h>


////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Deklaracja typu. Trzeba go specjalizowa� za pomoc� makr. Ta wersja b�dzie
//! rzuca� statyczn� asercj�.
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

//! Baza dla typu wrapuj�cego jaki� obiekt. Poza trzymaniem metadanych klasy pochodne
//! trzymaj� referencje do obiekt�w.
class ObjectWrapper
{
public:
    
    //! Lista typ�w.
    typedef std::list<TypeInfo> Types;

protected:
    //! Nazwa instancji.
    std::string name;
    //! Nazwa typu.
    std::string className;
    //! Hash identyfikuj�cy typ.
    std::size_t classID;
    //! Opis �r�d�a.
    std::string source;
    //! Czy obiekt uleg� zmianie?
    bool changed;

public:
    //! \param object
    //! \param name
    //! \return Wrapper obiektu.
    template <class T>
    static shared_ptr<ObjectWrapper> create(T* dummy = nullptr)
    {
        UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
        typedef ObjectWrapperT<T> Wrapper;
        Wrapper * wrapper = new Wrapper();
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
    ObjectWrapper(const char* className, const TypeInfo& type) :
    className(className), classID(type.hash_code()), changed(false)
    {}

public:
    //! Pusty polimorficzny destruktor.
    virtual ~ObjectWrapper() {}

public:
    //! \param dummy Pozostawi� pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni� parametr szablonu rzucany jest wyj�tek.
    template <class T>
    typename ObjectWrapperT<T>::Ptr get(bool exact = false, T* /*dummy*/ = nullptr)
    {   
        typedef ObjectWrapperT<T> Wrapper;
        Wrapper::Ptr result;
        if ( tryGet<T>(result) ) {
            return result;
        } else {
            throw std::runtime_error("Invalid wrapped type.");
        }
    }

    //! \param dummy Pozostawi� pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni� parametr szablonu rzucany jest wyj�tek.
    template <class T>
    typename ObjectWrapperT<typename boost::remove_const<T>::type>::ConstPtr get(bool exact = false, T* /*dummy*/ = nullptr) const
    {   
        typedef typename boost::remove_const<T>::type MutableT;
        typedef ObjectWrapperT<MutableT> Wrapper;
        Wrapper::ConstPtr result;
        if ( tryGet<MutableT>(result) ) {
            return result;
        } else {
            throw std::runtime_error("Invalid wrapped type.");
        }
    }


    //! \param dummy Pozostawi� pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni� parametr szablonu lub nie mozna wykonacz rzutowania dynamicznego zwracany jest false.
    template <class T>
    bool tryGet( typename ObjectWrapperT<T>::Ptr& target, bool exact = false )
    {
        typedef ObjectWrapperT<T> Wrapper;
        if ( exact ) {
            if ( getTypeInfo() == typeid(T) ) {
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

    //! \param dummy Pozostawi� pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni� parametr szablonu lub nie mozna wykonacz rzutowania dynamicznego zwracany jest false.
    template <class T>
    bool tryGet( typename ObjectWrapperT<T>::ConstPtr& target, bool exact = false ) const
    {
        typedef const ObjectWrapperT<T> Wrapper;
        if ( exact ) {
            if ( getTypeInfo() == typeid(T) ) {
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


    //! \param object Powinien to by� smart_ptr do obiektu domenowego, inaczej statyczna asercja  podczas kompilacji
    //! Tylko smart_ptr mo�na przekaza� gdy� przy surowym wska�niku powstan� nam 2 liczniki referencji i jeden mo�e zwolni� obiek nie informuj�c drugiego.    
    template <class TPtr>
    void set( const TPtr& object )
    {
        set(object, boost::is_pointer<TPtr>());
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
    //! \return
    inline const std::string& getSource() const
    { 
        return source;
    }
    //! \param source
    void setSource(const std::string& source) 
    { 
        this->source = source; 
    }

    //! \param type Typ do por�wnania.
    //! \return Czy to te same typy?
    inline bool isTypeEqual(const TypeInfo& type)
    {
        return getTypeInfo() == type;
    }

    //!
    inline TypeInfo getType() const
    {
        return getTypeInfo();
    }

    //! \return
    bool isChanged() const
    { 
        return changed;
    }
    //! \param changed
    void setChanged(bool changed) 
    { 
        this->changed = changed; 
    }


    //! \param type 
    //! \return Czy obiekt wspiera okre�lony typ?
    virtual bool isSupported(const TypeInfo& type) const = 0;

    //! \return Informacje o typie.
    virtual TypeInfo getTypeInfo() const = 0;

    //! \param supported Lista wspieranych rozszerze�.
    virtual void getSupportedTypes(Types& supported) const = 0;

    //! \return Czy wrappowany obiekt jest wyzerowany?
    virtual bool isNull() const = 0;

private:

    template <class TPtr>
    void set( const TPtr& object, boost::true_type )
    {
        UTILS_STATIC_ASSERT( false, "Nie moze by� surowy wskaznik!" );
    }

    template <class TPtr>
    void set( const TPtr& object, boost::false_type )
    {
        typedef TPtr::element_type T;
        typedef ObjectWrapperT<T> Wrapper;
        Wrapper* wrapper = dynamic_cast<Wrapper*>(this);
        if ( !wrapper ) {
            throw std::runtime_error("Invalid wrapped type.");
        } else {
            return wrapper->set(object);
        }
    }
};

typedef shared_ptr<ObjectWrapper> ObjectWrapperPtr;
typedef shared_ptr<const ObjectWrapper> ObjectWrapperConstPtr;
typedef weak_ptr<ObjectWrapper> ObjectWrapperWeakPtr;

//! Pomocniczy typ bazowy, zarz�dzaj�cy obiektem za pomoc� parametru
//! PtrPolicy. Tego typu nigdy nie u�ywa si� wprost.
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
    //! Sta�y wrappowany obiekt
    typedef typename Base::Ptr<const T>::Type ConstPtr;

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
    ConstPtr get() const
    {
        return ConstPtr(wrapped);
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
    virtual TypeInfo getTypeInfo() const
    {
        return typeid(T);
    }
    //! \param type 
    //! \return Czy obiekt wspiera okre�lony typ?
    virtual bool isSupported(const TypeInfo& type) const
    {
        return type == typeid(T);
    }
    //! \param supported
    virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
    {
        supported.push_back(typeid(T));
    }
    //! \return Czy wewn�trzny wska�nik jest wyzerowany?
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
    //! U�ycie PtrPolicy typu bazowego.
    typedef typename Base::PtrPolicy PtrPolicy;
    //! U�ywany wska�nik.
    typedef typename PtrPolicy::Ptr<T>::Type Ptr;
    //! 
    typedef typename PtrPolicy::Ptr<const T>::Type ConstPtr;
public:
    //! \param wrapped
    __ObjectWrapperT(const char* className)
    : Base(className)
    {}
public:
    //! \return Wrappowany obiekt.
    ConstPtr get() const
    {
        return ConstPtr(static_pointer_cast<T>(this->wrapped));
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
    virtual TypeInfo getTypeInfo() const
    {
        return typeid(T);
    }
    //! \param type 
    //! \return Czy obiekt wspira okre�lony typ?
    virtual bool isSupported(const TypeInfo& type) const
    {
        if ( type == typeid(T) ) {
            return true;
        } else {
            return Base::isSupported(type);
        }
    }
    //! \param supported
    virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
    {
        Base::getSupportedTypes(supported);
        supported.push_back(typeid(T));
    }
};

//! Typ wyznaczaj�cy baz� dla wrapper�w chc�cych dziedzyczy� po:
//! 1) PtrPolicy (ten wariant)
//! 2) Wrapperach dla typ�w (specjalizacje w makrze)
template <class T> 
struct __ObjectWrapperBaseSelector
{
    typedef T Type;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

// Makro tworz�ce specjalizacj� ObjectWrapperT. Musi wyst�powa� w globalnym
// namespace. Drugim parametrem mo�e by� dowolny typ maj�cy cechy ptrPolicy,
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