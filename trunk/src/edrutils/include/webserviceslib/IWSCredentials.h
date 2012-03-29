/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   9:44
    filename: IWsdlServiceCredentials.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IWSCREDENTIALS_H__
#define HEADER_GUARD___IWSCREDENTIALS_H__

#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>
#include <string>
#include <boost/shared_ptr.hpp>

namespace webservices
{

//! Interfejs konfigurowania autoryzacji u�ytkownika us�ug webowych
class IWSCredentials : public OpenThreads::ReentrantMutex
{
public:

	typedef OpenThreads::ReentrantMutex Mutex;
	typedef OpenThreads::ScopedLock<Mutex> ScopedLock;

public:

    //! Wirtualny destruktor
    virtual ~IWSCredentials() {}

    //! \param name Nazwa u�ytkownika wywo�uj�cego us�ug�
    virtual void setUser(const std::string & user) = 0;

    //! \param password Has�o u�ytkownika wywo�uj�cego us�ug�
    virtual void setPassword(const std::string & password) = 0;
    //! \param name Nazwa u�ytkownika wywo�uj�cego us�ug�
    //! \param password Has�o u�ytkownika wywo�uj�cego us�ug�
    virtual void setCredentials(const std::string & user, const std::string & password) = 0;

    //! \return Nazwa u�ytkownika kt�ry wywo�uje us�ug�
    virtual const std::string user() const = 0;
    //! \return Has�o u�ytkownika kt�ry wywo�uje us�ug�
    virtual const std::string password() const = 0;
};

typedef boost::shared_ptr<IWSCredentials> WSCredentialsPtr;
typedef boost::shared_ptr<const IWSCredentials> WSCredentialsConstPtr;

}

#endif	//	HEADER_GUARD___IWSCREDENTIALS_H__
