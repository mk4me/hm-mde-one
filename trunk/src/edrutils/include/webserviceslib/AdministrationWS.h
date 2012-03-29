/********************************************************************
    created:  2012/03/02
    created:  2:3:2012   14:30
    filename: AdministrationWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ADMINISTRATIONWS_H__
#define HEADER_GUARD___ADMINISTRATIONWS_H__

#include <webserviceslib/IAdministrationWS.h>

namespace webservices
{
    class AdministrationWS : public IAdministrationWS
    {
    public:

		AdministrationWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~AdministrationWS();

        //! \param connection Po³¹czenie przez które bêdzie realizowany serwis
        virtual void setConnection(const WSConnectionPtr & connection);
        //! \return Po³¹czenie przez które jest realizowany serwis
        virtual const WSConnectionPtr & connection();
        //! \return Po³¹czenie przez które jest realizowany serwis
        virtual const WSConnectionConstPtr & connection() const;

        virtual void defineAttributeGroup(const std::string & groupName, const std::string & entity);

        virtual void removeAttributeGroup(const std::string & groupName, const std::string & entity);

        virtual void defineAttribute(const std::string & attributeName, const std::string & groupName,
            const std::string & entity, bool isEnum, const std::string & pluginDescriptor,
            const std::string & type, const std::string & unit);

        virtual void removeAttribute(const std::string & attributeName, const std::string & groupName,
            const std::string & entity);

        virtual void addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
            const std::string & entity, const std::string & value, bool clearExisting);

        virtual void downloadAreaCleanup(int olderThanMinutes);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };
}

#endif	//	HEADER_GUARD___ADMINISTRATIONWS_H__
