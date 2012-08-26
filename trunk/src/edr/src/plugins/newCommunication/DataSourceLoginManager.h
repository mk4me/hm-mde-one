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

//! Klasa zarządzająca logowaniem użytkownika
class DataSourceLoginManager
{
public:
    //! Domyślny konstruktor
    DataSourceLoginManager();
    //! Destruktor
    ~DataSourceLoginManager();

    //! \param user Nazwa użytkownika w bazie danych
    //! \param password Hasło użytkownika w bazie danych
    void login(const std::string & user, const std::string & password);
    //! Wylogowanie użytkownika
    void logout();

    //! \return Aktualnie zalogowany użytkownik
    const User & user() const;

    //! \return Czy udało się zalogować
    const bool isLogged() const;

private:
    //! \param userID Identyfikator użytkownika dla którego chcemy pobrać specyficzne dane
    //! \param userData [out] Dane użytkownika zapisane w bazie
    static void getUserData(int userID, UserData & userData);
    //! \param userID Identyfikator użytkownika dla którego chcemy pobrać prawa do przeglądania danych w bazie danych
    //! \param userPrivilages [out] Uprawnienia użytkownika do danych
    static void getUserPrivilages(int userID, UserPrivilages & userPrivilages);
    //! \param login Login użytkownika w bazie (najprawdopodobniej jego email)
    //! \param password Hasło użytkownika w bazie
    //! \return Identyfikator użytkownika lub -1 jeżeli nie znaleziono
    static const int getUserIDForLoginAndPassword(const std::string & login, const std::string & password);

private:
    //! Identyfikator aktualnie zalogowanego użytkownika
    User user_;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCELOGIN_H__
