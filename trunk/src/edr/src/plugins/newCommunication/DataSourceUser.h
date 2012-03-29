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

//! Implementacja obsługująca informacje o aktualnym użytkowniku
class User : public communication::IUser
{
public:

    //! Konstruktor domyslny
    User();

    //! Destruktor wirtualny
    virtual ~User();

    //! \return Identyfikator użytkownika
    virtual const int id() const;
    //! \param id Nazwa użytkownika
    void setID(int id);

    //! \return Nazwa użytkownika
    virtual const std::string & name() const;
    //! \param name Nazwa użytkownika
    void setName(const std::string & name);

    //! \return Hasło użytkownika
    const std::string & password() const;
    //! \param password Hasło użytkownika
    void setPassword(const std::string & password);

    //! \return Prawa uzytkownika
    virtual const UserPrivilages & userPrivilages() const;
    //! \param userPrivilages Nowe uprawnienia użytkownika
    void setPrivilages(const UserPrivilages & userPrivilages);

    //! \return Dane użytkownika
    virtual const UserData & userData() const;
    //! \param userData Nowe dane użytkownika
    void setData(const UserData & userData);

private:
    //! Identyfikator użytkownika
    int id_;
    //! Nazwa użytkownika
    std::string name_;
    //! Hasło użytkownika
    std::string password_;
    //! Uprawnienia użytkownika
    UserPrivilages privilages_;
    //! Dane użytkownika
    UserData data_;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCEUSER_H__