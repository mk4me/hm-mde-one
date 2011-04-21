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
    
    //! Lista typów.
    typedef std::list<TypeInfo> Types;

protected:
    //! Nazwa instancji.
    std::string name;
    //! Nazwa typu.
    std::string className;
    //! Hash identyfikuj¹cy typ.
    std::size_t classID;
    //! Opis Ÿród³a.
    std::string source;
    //! Czy obiekt uleg³ zmianie?
    bool changed;

public:
    //! Return type resolver
    struct get_t
    {
        ObjectWrapper* wrapper;
        const ObjectWrapper* constWrapper;
        bool exact;
        
        template <class Ptr>
        operator Ptr()
        {
            Ptr result;
            if (wrapper->tryGet(result, exact)) {
                return result;
            } else {
                throw std::bad_cast("Invalid cast");
            }
        }
        template <class Ptr>
        operator Ptr() const
        {
            Ptr result;
            if (constWrapper->tryGet(result, exact)) {
                return result;
            } else {
                throw std::bad_cast("Invalid cast");
            }
        }
    };

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

    //! Próba pobrania obiektu z wrappera.
    //! \param object Rezultat.
    //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
    //! \return Sukces/pora¿ka.
    template <class Ptr>
    bool tryGet(Ptr& object, bool exact = false)
    {
        TypeInfo type = typeid(Ptr);
        if ( exact ) {
            auto types = getPtrTypeInfo();
            if ( types.first == type || types.second == type ) {
                return getSmartPtr(&object, type);
            } else {
                return false;
            }
        } else {
            return getSmartPtr(&object, type);
        }
    }

    //! Próba pobrania obiektu z wrappera.
    //! \param object Rezultat.
    //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
    //! \return Sukces/pora¿ka.
    template <class Ptr>
    bool tryGet(Ptr& object, bool exact = false) const
    {
        TypeInfo type = typeid(Ptr);
        if ( exact ) {
            auto types = getPtrTypeInfo();
            if ( types.second == type ) {
                return getSmartPtr(&object, type);
            } else {
                return false;
            }
        } else {
            return getSmartPtr(&object, type);
        }
    }

    //! Pobiera obiekt z wrappera. W razie b³êdu rzuca bad_castem.
    //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
    //! \return Return Type Resolver
    get_t get(bool exact = false)
    {
        get_t result = { this, nullptr, exact };
        return result;
    }

    //! \param dummy Pozostawiæ pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
    template <class T>
    typename ObjectWrapperT<T>::Ptr get(T* /*dummy*/ = nullptr)
    {   
        typedef ObjectWrapperT<T> Wrapper;
        return static_cast<Wrapper::Ptr>(get());
    }

    //! Pobiera obiekt z wrappera. W razie b³êdu rzuca bad_castem.
    //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
    //! \return Return Type Resolver
    const get_t get(bool exact = false) const
    {
        const get_t result = { nullptr, this, exact };
        return result;
    }

    //! Ustawia obiekt wrappera. W razie b³êdu rzuca bad_castem.
    //! \param object Obiekt.
    template <class Ptr>
    void set(Ptr& object)
    {
        if ( !trySet(object) ) {
            throw std::bad_cast("Invalid cast");
        }
    }

    //! Ustawia obiekt wrappera.
    //! \param object Obiekt.
    //! \return Sukces/pora¿ka.
    template <class Ptr>
    bool trySet(Ptr& object)
    {
        TypeInfo type = typeid(Ptr);
        if ( getPtrTypeInfo().first == type ) {
            return setSmartPtr(&object, type);
        } else {
            return false;
        }
    }
// 
// 
//     //! \param dummy Pozostawiæ pusty.
//     //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
//     template <class T>
//     typename ObjectWrapperT<T>::Ptr get(bool exact = false, T* /*dummy*/ = nullptr)
//     {   
//         typedef ObjectWrapperT<T> Wrapper;
//         Wrapper::Ptr result;
//         if ( tryGet<T>(result) ) {
//             return result;
//         } else {
//             throw std::runtime_error("Invalid wrapped type.");
//         }
//     }
// 
//     //! \param dummy Pozostawiæ pusty.
//     //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
//     template <class T>
//     typename ObjectWrapperT<typename boost::remove_const<T>::type>::ConstPtr get(bool exact = false, T* /*dummy*/ = nullptr) const
//     {   
//         typedef typename boost::remove_const<T>::type MutableT;
//         typedef ObjectWrapperT<MutableT> Wrapper;
//         Wrapper::ConstPtr result;
//         if ( tryGet<MutableT>(result) ) {
//             return result;
//         } else {
//             throw std::runtime_error("Invalid wrapped type.");
//         }
//     }
// 
// 
//     //! \param dummy Pozostawiæ pusty.
//     //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu lub nie mozna wykonacz rzutowania dynamicznego zwracany jest false.
//     template <class T>
//     bool tryGet( typename ObjectWrapperT<T>::Ptr& target, bool exact = false )
//     {
//         typedef ObjectWrapperT<T> Wrapper;
//         if ( exact ) {
//             if ( getTypeInfo() == typeid(T) ) {
//                 target = static_cast<Wrapper*>(this)->get();
//                 return true;
//             }
//         } else {
//             Wrapper* wrapper = dynamic_cast<Wrapper*>(this);
//             if ( wrapper ) {
//                 target = wrapper->get();
//                 return true;
//             }
//         }
//         return false;
//     }
// 
//     //! \param dummy Pozostawiæ pusty.
//     //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu lub nie mozna wykonacz rzutowania dynamicznego zwracany jest false.
//     template <class T>
//     bool tryGet( typename ObjectWrapperT<T>::ConstPtr& target, bool exact = false ) const
//     {
//         typedef const ObjectWrapperT<T> Wrapper;
//         if ( exact ) {
//             if ( getTypeInfo() == typeid(T) ) {
//                 target = static_cast<Wrapper*>(this)->get();
//                 return true;
//             }
//         } else {
//             Wrapper* wrapper = dynamic_cast<Wrapper*>(this);
//             if ( wrapper ) {
//                 target = wrapper->get();
//                 return true;
//             }
//         }
//         return false;
//     }
// 
// 
//     //! \param object Powinien to byæ smart_ptr do obiektu domenowego, inaczej statyczna asercja  podczas kompilacji
//     //! Tylko smart_ptr mo¿na przekazaæ gdy¿ przy surowym wskaŸniku powstan¹ nam 2 liczniki referencji i jeden mo¿e zwolniæ obiek nie informuj¹c drugiego.    
//     template <class TPtr>
//     void set( const TPtr& object )
//     {
//         set(object, boost::is_pointer<TPtr>());
//     }

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

    //! \param type Typ do porównania.
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
    //! \return Czy obiekt wspiera okreœlony typ?
    virtual bool isSupported(const TypeInfo& type) const = 0;
    //! \param type Typ inteligentnego wskaŸnika.
    //! \return true je¿eli do z obiektu mo¿na wy³uskaæ dany wskaŸnik.
    virtual bool isPtrSupported(const TypeInfo& type) const = 0;

    //! \return Informacje o typie.
    virtual TypeInfo getTypeInfo() const = 0;

    //! \return Informacje o typie odpowiednio normalnego i sta³ego wskaŸnika.
    virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const = 0;

    //! \param supported Lista wspieranych rozszerzeñ.
    virtual void getSupportedTypes(Types& supported) const = 0;

    //! \return Czy wrappowany obiekt jest wyzerowany?
    virtual bool isNull() const = 0;

private:

    //! \return Czy uda³o siê ustawiæ m¹dry wskaŸnik?
    virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const = 0;

    virtual bool setSmartPtr(void* ptr, const TypeInfo& type) = 0;
// 
//     template <class TPtr>
//     void set( const TPtr& object, boost::true_type )
//     {
//         UTILS_STATIC_ASSERT( false, "Nie moze byæ surowy wskaznik!" );
//     }
// 
//     template <class TPtr>
//     void set( const TPtr& object, boost::false_type )
//     {
//         typedef TPtr::element_type T;
//         typedef ObjectWrapperT<T> Wrapper;
//         Wrapper* wrapper = dynamic_cast<Wrapper*>(this);
//         if ( !wrapper ) {
//             throw std::runtime_error("Invalid wrapped type.");
//         } else {
//             return wrapper->set(object);
//         }
//     }
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
    //! Sta³y wrappowany obiekt
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
    //! \return Informacja o typie.
    virtual TypeInfo getTypeInfo() const
    {
        return typeid(T);
    }
    virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const
    {
        return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));
    }

    //! \param type 
    //! \return Czy obiekt wspiera okreœlony typ?
    virtual bool isSupported(const TypeInfo& type) const
    {
        return type == typeid(T);
    }
    //! \param supported
    virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
    {
        supported.push_back(typeid(T));
    }
    //! \return Czy wewnêtrzny wskaŸnik jest wyzerowany?
    virtual bool isNull() const
    {
        return wrapped ? false : true;
    }

    virtual bool isPtrSupported(const TypeInfo& type) const
    {
        return type == typeid(Ptr) || type == typeid(ConstPtr);
    }

    virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const
    {
        if ( type == typeid(Ptr) ) {
            PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), wrapped);
            return true;
        } else if ( type == typeid(ConstPtr) ) {
            PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(wrapped));
            return true;
        }
        return false;
    }

    virtual bool setSmartPtr(void* ptr, const TypeInfo& type)
    {
        if ( type == typeid(Ptr) ) {
            PtrPolicy::setPtr(wrapped, *reinterpret_cast<Ptr*>(ptr));
            return true;
        } else {
            return false;
        }
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
    //! 
    typedef typename PtrPolicy::Ptr<const T>::Type ConstPtr;
public:
    //! \param wrapped
    __ObjectWrapperT(const char* className)
    : Base(className)
    {}
public:
    //! \return Informacja o typie.
    virtual TypeInfo getTypeInfo() const
    {
        return typeid(T);
    }

    //! \param type 
    //! \return Czy obiekt wspira okreœlony typ?
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

    //! \see ObjectWrapper::getPtrTypeInfo
    virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const
    {
        return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));
    }

    //! \see ObjectWrapper::getSmartPtr
    virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const
    {
        if ( type == typeid(Ptr) ) {
            PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), static_pointer_cast<T>(this->wrapped));
            return true;
        } else if ( type == typeid(ConstPtr) ) {
            PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(static_pointer_cast<T>(this->wrapped)));
            return true;
        }
        return Base::getSmartPtr(ptr, type);
    }

    virtual bool setSmartPtr(void* ptr, const TypeInfo& type)
    {
        if ( type == typeid(Ptr) ) {
            PtrPolicy::setPtr(static_pointer_cast<T>(this->wrapped), *reinterpret_cast<Ptr*>(ptr));
            return true;
        } else {
            return Base::getSmartPtr(ptr, type);
        }
    }

    virtual bool isPtrSupported(const TypeInfo& type) const
    {
        return type == typeid(Ptr) || type == typeid(ConstPtr) || Base::isPtrSupported(type);
    }

};

//! Typ wyznaczaj¹cy bazê dla wrapperów chc¹cych dziedzyczyæ po:
//! 1) PtrPolicy (ten wariant)
//! 2) Wrapperach dla typów (specjalizacje w makrze)
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