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

    typedef std::vector<core::ObjectWrapperConstPtr>::const_iterator const_iterator;

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

    template <class Ptr>
    bool tryGet(Ptr& object, bool exact = false) const
    {
        for(auto it = begin(); it !+ end(); it++){
            if ((*it)->tryGet(object, exact)) {
                return true;
            }
        }

        return false;
    }

    template<class Channel, class Collection>
    void getWrappersFromCollection(const Collection& collection, std::vector<core::ObjectWrapperConstPtr> & channes) const
    {
        Collection& tempCollection = const_cast<Collection&>(collection);
        int count = tempCollection.getNumChannels();
        for (int i = 0; i < count; i++) {
            Channel c = boost::dynamic_pointer_cast<Channel::element_type>(tempCollection.getChannel(i));
            for(auto i = 0; i < size() ; i++){
                Channel wc;
                if (get(i)->tryGet(wc)) {
                    if (wc == c) {
                        channes.push_back(*it);
                    }
                }
            }
        }
    }

    virtual bool hasObjectOfType(const core::TypeInfo& type, bool exact = false) const
    {
        for(int i = 0; i < size(); i++){
            auto obj = get(i);
            
            if(obj->getTypeInfo() == type || (exact == false && obj->isSupported(type))){
                return true;
            }
        }

        return false;
    }

    virtual core::ObjectWrapperConstPtr getWrapperOfType(const core::TypeInfo& type, bool exact = false) const
    {
        for(int i = 0; i < size(); i++){
            auto obj = get(i);
            
            if(obj->getTypeInfo() == type || (exact == false && obj->isSupported(type))){
                return obj;
            }
        }

        throw std::runtime_error("Object type not stored in Motion");
    }

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const
    {
        for(int i = 0; i < size(); i++){
            wrappers.push_back(get(i));
        }
    }

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers, const core::TypeInfo& type, bool exact = false) const
    {
        for(int i = 0; i < size(); i++){
            auto obj = get(i);

            if(obj->getTypeInfo() == type || (exact == false && obj->isSupported(type))){
                wrappers.push_back(obj);
            }
        }
    }

    virtual int size() const = 0;
    virtual const core::ObjectWrapperConstPtr & get(int i) const = 0;
};

CORE_DEFINE_WRAPPER(IMotion, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___IMOTION_H__