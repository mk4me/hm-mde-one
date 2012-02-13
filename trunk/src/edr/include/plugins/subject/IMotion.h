/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:02
    filename: IMotion.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IMOTION_H__
#define HEADER_GUARD___IMOTION_H__

#include <plugins/subject/Types.h>
#include <core/ObjectWrapper.h>
#include <plugins/subject/ISession.h>

//! Abstrakcyjna klasa reprezentuj¹ca pojedynczy ruch obiektu w ramach sesji
class IMotion
{
public:

    virtual ~IMotion() {}

    //! \return Globalny, unikalny ID ruchu
    virtual SubjectID getID() const = 0;
    //! \return Lokalny, unikalny ID w ramach sesji
    virtual SubjectID getLocalID() const = 0;
    //! \return Sesja z któr¹ zwi¹zany jest ten ruch
    virtual const SessionConstPtr & getSession() const = 0;

    //! \return Globalna nazwa ruchu
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa ruchu w ramach sesji
    virtual const std::string & getLocalName() const = 0;

    //! \return Iloœæ obiektów domenowych Motiona
    virtual int size() const = 0;
    //! \param i Indeks obiektu domenowego który chcemy pobraæ [0 - (size() - 1)]
    //! \return Obiekt domenowy
    virtual const core::ObjectWrapperConstPtr & get(int i) const = 0;

    template <class Ptr>
    bool tryGet(Ptr& object, bool exact = false) const
    {
        auto s = size();
        for(int i = 0; i < s; ++i){
            if (get(i)->tryGet(object, exact)) {
                return true;
            }
        }

        return false;
    }

    template<class Channel, class Collection>
    void getWrappersFromCollection(const Collection& collection, std::vector<core::ObjectWrapperConstPtr> & channels) const
    {
        Collection& tempCollection = const_cast<Collection&>(collection);
        int count = tempCollection.getNumChannels();
        auto s = size();
        for (int i = 0; i < count; ++i) {
            Channel c = boost::dynamic_pointer_cast<Channel::element_type>(tempCollection.getChannel(i));
            for(auto j = 0; j < s ; ++j){
                Channel wc;
                auto obj = get(j);
                if (obj->tryGet(wc)) {
                    if (wc == c) {
                        channels.push_back(obj);
                    }
                }
            }
        }
    }

    virtual bool hasObjectOfType(const core::TypeInfo& type, bool exact = false) const
    {
        auto s = size();
        for(int i = 0; i < s; ++i){
            auto obj = get(i);
            
            if(obj->getTypeInfo() == type || (exact == false && obj->isSupported(type))){
                return true;
            }
        }

        return false;
    }

    virtual core::ObjectWrapperConstPtr getWrapperOfType(const core::TypeInfo& type, bool exact = false) const
    {
        auto s = size();
        for(int i = 0; i < s; ++i){
            auto obj = get(i);
            
            if(obj->getTypeInfo() == type || (exact == false && obj->isSupported(type))){
                return obj;
            }
        }

        throw std::runtime_error("Object type not stored in Motion");
    }

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const
    {
        auto s = size();
        for(int i = 0; i < s; ++i){
            wrappers.push_back(get(i));
        }
    }

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers, const core::TypeInfo& type, bool exact = false) const
    {
        auto s = size();
        for(int i = 0; i < s; ++i){
            auto obj = get(i);

            if(obj->getTypeInfo() == type || (exact == false && obj->isSupported(type))){
                wrappers.push_back(obj);
            }
        }
    }
};

CORE_DEFINE_WRAPPER(IMotion, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___IMOTION_H__