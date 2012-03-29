/********************************************************************
    created:  2012/02/03
    created:  3:2:2012   17:11
    filename: CommunicationDataSourceLogin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCELOGIN_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCELOGIN_H__

#include <string>
#include "DataSourceUser.h"

//! Klasa zarz�dzaj�ca logowaniem u�ytkownika
class DataSourceLoginManager
{
public:
    //! Domy�lny konstruktor
    DataSourceLoginManager();
    //! Destruktor
    ~DataSourceLoginManager();

    //! \param user Nazwa u�ytkownika w bazie danych
    //! \param password Has�o u�ytkownika w bazie danych
    void login(const std::string & user, const std::string & password);
    //! Wylogowanie u�ytkownika
    void logout();

    //! \return Aktualnie zalogowany u�ytkownik
    const User & user() const;

    //! \return Czy uda�o si� zalogowa�
    const bool isLogged() const;

private:
    //! \param userID Identyfikator u�ytkownika dla kt�rego chcemy pobra� specyficzne dane
    //! \param userData [out] Dane u�ytkownika zapisane w bazie
    static void getUserData(int userID, UserData & userData);
    //! \param userID Identyfikator u�ytkownika dla kt�rego chcemy pobra� prawa do przegl�dania danych w bazie danych
    //! \param userPrivilages [out] Uprawnienia u�ytkownika do danych
    static void getUserPrivilages(int userID, UserPrivilages & userPrivilages);
    //! \param login Login u�ytkownika w bazie (najprawdopodobniej jego email)
    //! \param password Has�o u�ytkownika w bazie
    //! \return Identyfikator u�ytkownika lub -1 je�eli nie znaleziono
    static const int getUserIDForLoginAndPassword(const std::string & login, const std::string & password);

private:
    //! Identyfikator aktualnie zalogowanego u�ytkownika
    User user_;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCELOGIN_H__