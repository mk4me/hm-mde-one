/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   20:55
    filename: IDataSourceUser.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEUSER_H__
#define HEADER_GUARD___IDATASOURCEUSER_H__

#include <plugins/newCommunication/IDataSourceUserData.h>
#include <plugins/newCommunication/IDataSourceUserPrivilages.h>

namespace communication
{

//! Interfejs klasy z danymi uzytkownika
class IUser
{
public:
    //! Destruktor wirtualny
    virtual ~IUser() {}

    //! \return Identyfikator u¿ytkownika
    virtual const int id() const = 0;

    //! \return Nazwa u¿ytkownika z bazy
    virtual const std::string & name() const = 0;

    //! \return Prawa uzytkownika
    virtual const IUserPrivilages & userPrivilages() const = 0;
    //! \return Dane u¿ytkownika
    virtual const IUserData & userData() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCEUSER_H__
