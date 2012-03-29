/********************************************************************
    created:  2012/02/03
    created:  3:2:2012   17:10
    filename: CommunicationDataSourceUser.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCEUSER_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCEUSER_H__

#include <plugins/newCommunication/IDataSourceUser.h>
#include "DataSourceUserData.h"
#include "DataSourceUserPrivilages.h"

//! Implementacja obs³uguj¹ca informacje o aktualnym u¿ytkowniku
class User : public communication::IUser
{
public:

    //! Konstruktor domyslny
    User();

    //! Destruktor wirtualny
    virtual ~User();

    //! \return Identyfikator u¿ytkownika
    virtual const int id() const;
    //! \param id Nazwa u¿ytkownika
    void setID(int id);

    //! \return Nazwa u¿ytkownika
    virtual const std::string & name() const;
    //! \param name Nazwa u¿ytkownika
    void setName(const std::string & name);

    //! \return Has³o u¿ytkownika
    const std::string & password() const;
    //! \param password Has³o u¿ytkownika
    void setPassword(const std::string & password);

    //! \return Prawa uzytkownika
    virtual const UserPrivilages & userPrivilages() const;
    //! \param userPrivilages Nowe uprawnienia u¿ytkownika
    void setPrivilages(const UserPrivilages & userPrivilages);

    //! \return Dane u¿ytkownika
    virtual const UserData & userData() const;
    //! \param userData Nowe dane u¿ytkownika
    void setData(const UserData & userData);

private:
    //! Identyfikator u¿ytkownika
    int id_;
    //! Nazwa u¿ytkownika
    std::string name_;
    //! Has³o u¿ytkownika
    std::string password_;
    //! Uprawnienia u¿ytkownika
    UserPrivilages privilages_;
    //! Dane u¿ytkownika
    UserData data_;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCEUSER_H__