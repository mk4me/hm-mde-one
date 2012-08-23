/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   20:55
    filename: IDataSourceUser.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEUSER_H__
#define HEADER_GUARD___IDATASOURCEUSER_H__

#include <string>
#include <plugins/newCommunication/IDataSourceUserData.h>
#include <plugins/newCommunication/IDataSourceUserPrivilages.h>

namespace communication
{

//! Interfejs klasy z danymi u퓓tkownika
class IUser
{
public:
    //! Destruktor wirtualny
    virtual ~IUser() {}

    //! \return Identyfikator u퓓tkownika
    virtual const int id() const = 0;

    //! \return Nazwa u퓓tkownika z bazy
    virtual const std::string & name() const = 0;

    //! \return Prawa u퓓tkownika
    virtual const IUserPrivilages & userPrivilages() const = 0;
    //! \return Dane u퓓tkownika
    virtual const IUserData & userData() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCEUSER_H__
