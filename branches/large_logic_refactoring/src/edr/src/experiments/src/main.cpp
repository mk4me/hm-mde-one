#include <iostream>
#include <list>
#include <string>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
//#include <core/TypeInfo.h>
//#include <utils/Debug.h>
//#include <core/SmartPtr.h>
//#include <utils/PtrPolicyBoost.h>

using namespace std;


//using namespace osg;
struct NullType {};
struct PtrPolicyBoost
{
    //! Typ wskaźnika.
    template <class T> struct Ptr 
    {
        typedef boost::shared_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
    template <class T> struct Pointed;
    //! Specjalizacja wyciągająca wskaźnik.
    template <class T> struct Pointed< boost::shared_ptr<T> >
    {
        typedef typename boost::shared_ptr<T>::element_type Type;
    };

    //! Klasa bazowa.
    typedef NullType Base;

    //! Zerowanie wskaźnika.
    //! \param ptr
    template <class T>
    void initPtr( T & ptr ) const
    {}

    //! Ustawienie wartości wskaźnika surowymi danymi.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(boost::shared_ptr<T> & ptr, T * data) const
    {
        ptr.reset(data);
    }

    //! Ustawienie wartości wskaźnika poprzes kopiowanie z innego mądrego wskaźnika.
    //! \param ptr
    //! \param data
    template<class T>
    void setPtr(boost::shared_ptr<T> & ptr, const boost::shared_ptr<T> & data) const
    {
        ptr = data;
    }

    //! Czy wskaźnik jest unikatowy?
    //! \param ptr
    template<class T>
    bool isUnique(boost::shared_ptr<T> & ptr) const
    {
        return ptr.unique();
    }
};

//template <> struct is_ptr_policy<PtrPolicyBoost> : public __traits::true_type {};



class TypeInfo
{
public:
    inline bool operator==(const TypeInfo& obj) const
    {
        return true;
    }

    //! \param rhs
    inline bool operator==(const std::type_info& rhs) const
    {
        return true;
    }

    //! \param rhs
    inline bool operator!=(const std::type_info& rhs) const
    {
        return !(*this == rhs);
    }
    //! Wewnętrzna informacja o typie.
    const std::type_info& typeinfo;
    //! Jawny operator rzutowania na type_info.
    inline operator const std::type_info&() const
    {
        return typeinfo;
    }
    //! \param typeinfo
    TypeInfo(const std::type_info& typeinfo) : typeinfo(typeinfo)
    {

    }
};

//! Deklaracja typu. Trzeba go specjalizować za pomocą makr. Ta wersja będzie
//! rzucać statyczną asercją.
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

//! Baza dla typu wrapującego jakiś obiekt. Poza trzymaniem metadanych klasy pochodne
//! trzymają referencje do obiektów.
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
    //! Hash identyfikujący typ.
    std::size_t classID;
    //! Opis źródła.
    std::string source;
    //! Czy obiekt uległ zmianie?
    bool changed;
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
    className(className), classID(0), changed(false)
    {}

public:
    //! Pusty polimorficzny destruktor.
    virtual ~ObjectWrapper() {}

public:

    //! Próba pobrania obiektu z wrappera.
    //! \param object Rezultat.
    //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
    //! \return Sukces/porażka.
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
    //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
    //! \return Sukces/porażka.
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

    //! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
    //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
    //! \return Return Type Resolver
    get_t get(bool exact = false)
    {
        get_t result = { this, nullptr, exact };
        return result;
    }

    //! \param dummy Pozostawić pusty.
    //! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
    template <class T>
    typename ObjectWrapperT<T>::Ptr get(T* /*dummy*/ = nullptr)
    {   
        typedef ObjectWrapperT<T> Wrapper;
        return static_cast<Wrapper::Ptr>(get());
    }

    //! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
    //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
    //! \return Return Type Resolver
    const get_t get(bool exact = false) const
    {
        const get_t result = { nullptr, this, exact };
        return result;
    }

    //! Ustawia obiekt wrappera. W razie błędu rzuca bad_castem.
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
    //! \return Sukces/porażka.
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
    //! \return Czy obiekt wspiera określony typ?
    virtual bool isSupported(const TypeInfo& type) const = 0;
    //! \param type Typ inteligentnego wskaźnika.
    //! \return true jeżeli do z obiektu można wyłuskać dany wskaźnik.
    virtual bool isPtrSupported(const TypeInfo& type) const = 0;

    //! \return Informacje o typie.
    virtual TypeInfo getTypeInfo() const = 0;

    //! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
    virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const = 0;

    //! \param supported Lista wspieranych rozszerzeń.
    virtual void getSupportedTypes(Types& supported) const = 0;

    //! \return Czy wrappowany obiekt jest wyzerowany?
    virtual bool isNull() const = 0;

private:

    //! \return Czy udało się ustawić mądry wskaźnik?
    virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const = 0;

    virtual bool setSmartPtr(void* ptr, const TypeInfo& type) = 0;
};

typedef shared_ptr<ObjectWrapper> ObjectWrapperPtr;
typedef shared_ptr<const ObjectWrapper> ObjectWrapperConstPtr;
typedef weak_ptr<ObjectWrapper> ObjectWrapperWeakPtr;

//! Pomocniczy typ bazowy, zarządzający obiektem za pomocą parametru
//! PtrPolicy. Tego typu nigdy nie używa się wprost.
template <class T, class PtrPolicy>
class __ObjectWrapperT : public ObjectWrapper, protected PtrPolicy
{

public:
    //!
    typedef PtrPolicy PtrPolicy;
    //!
    typedef PtrPolicy Base;
    //! Wrappowany obiekt.
    typedef typename Base::Ptr<T>::Type Ptr;
    //! Stały wrappowany obiekt
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
    //! \return Czy obiekt wspiera określony typ?
    virtual bool isSupported(const TypeInfo& type) const
    {
        return type == typeid(T);
    }
    //! \param supported
    virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
    {
        supported.push_back(typeid(T));
    }
    //! \return Czy wewnętrzny wskaźnik jest wyzerowany?
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
//
//template <class T, class B>
//class __ObjectWrapperT<T, ObjectWrapperT<B> > : public ObjectWrapperT<B>
//{
//public:
//    //! Wrappowany obiekt.
//    typedef ObjectWrapperT<B> Base;
//    //! Użycie PtrPolicy typu bazowego.
//    typedef typename Base::PtrPolicy PtrPolicy;
//    //! Używany wskaźnik.
//    typedef typename PtrPolicy::Ptr<T>::Type Ptr;
//    //! 
//    typedef typename PtrPolicy::Ptr<const T>::Type ConstPtr;
//public:
//    //! \param wrapped
//    __ObjectWrapperT(const char* className)
//    : Base(className)
//    {}
//public:
//    //! \return Informacja o typie.
//    virtual TypeInfo getTypeInfo() const
//    {
//        return typeid(T);
//    }
//
//    //! \param type 
//    //! \return Czy obiekt wspira określony typ?
//    virtual bool isSupported(const TypeInfo& type) const
//    {
//        if ( type == typeid(T) ) {
//            return true;
//        } else {
//            return Base::isSupported(type);
//        }
//    }
//    //! \param supported
//    virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
//    {
//        Base::getSupportedTypes(supported);
//        supported.push_back(typeid(T));
//    }
//
//    //! \see ObjectWrapper::getPtrTypeInfo
//    virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const
//    {
//        return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));
//    }
//
//    //! \see ObjectWrapper::getSmartPtr
//    virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const
//    {
//        if ( type == typeid(Ptr) ) {
//            PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), static_pointer_cast<T>(this->wrapped));
//            return true;
//        } else if ( type == typeid(ConstPtr) ) {
//            PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(static_pointer_cast<T>(this->wrapped)));
//            return true;
//        }
//        return Base::getSmartPtr(ptr, type);
//    }
//
//    virtual bool setSmartPtr(void* ptr, const TypeInfo& type)
//    {
//        if ( type == typeid(Ptr) ) {
//            PtrPolicy::setPtr(static_pointer_cast<T>(this->wrapped), *reinterpret_cast<Ptr*>(ptr));
//            return true;
//        } else {
//            return Base::getSmartPtr(ptr, type);
//        }
//    }
//
//    virtual bool isPtrSupported(const TypeInfo& type) const
//    {
//        return type == typeid(Ptr) || type == typeid(ConstPtr) || Base::isPtrSupported(type);
//    }
//
//};

//! Typ wyznaczający bazę dla wrapperów chcących dziedzyczyć po:
//! 1) PtrPolicy (ten wariant)
//! 2) Wrapperach dla typów (specjalizacje w makrze)
template <class T> 
struct __ObjectWrapperBaseSelector
{
    typedef T Type;
};



struct ClonePolicyString
{
    typedef boost::shared_ptr<std::string> strPtr;
    strPtr clone(strPtr s) const {
        return strPtr(new std::string(*s));
    }
};


template <>                                                            
class ObjectWrapperT<std::string> : public __ObjectWrapperT<std::string,  
    __ObjectWrapperBaseSelector<PtrPolicyBoost>::Type>         
{                                                                      
public:                                                                
    static const char* className() { return "std::string"; }                   
public:                                                                
    ObjectWrapperT(const char* className = "std::string") :                    
    __ObjectWrapperT<std::string, __ObjectWrapperBaseSelector<PtrPolicyBoost>::Type>(className)        
{}                                                                  
};                                                                     
    template <>                                                        
struct __ObjectWrapperBaseSelector<std::string>                               
{                                                                      
    typedef ObjectWrapperT<std::string> Type;                                 
};

int main()
{
    
    return 0;
}
