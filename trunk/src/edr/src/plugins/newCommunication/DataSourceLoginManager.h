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

//! Klasa zarz¹dzaj¹ca logowaniem u¿ytkownika
class DataSourceLoginManager
{
public:
    //! Domyœlny konstruktor
    DataSourceLoginManager();
    //! Destruktor
    ~DataSourceLoginManager();

    //! \param user Nazwa u¿ytkownika w bazie danych
    //! \param password Has³o u¿ytkownika w bazie danych
    void login(const std::string & user, const std::string & password);
    //! Wylogowanie u¿ytkownika
    void logout();

    //! \return Aktualnie zalogowany u¿ytkownik
    const User & user() const;

    //! \return Czy uda³o siê zalogowaæ
    const bool isLogged() const;

private:
    //! \param userID Identyfikator u¿ytkownika dla którego chcemy pobraæ specyficzne dane
    //! \param userData [out] Dane u¿ytkownika zapisane w bazie
    static void getUserData(int userID, UserData & userData);
    //! \param userID Identyfikator u¿ytkownika dla którego chcemy pobraæ prawa do przegl¹dania danych w bazie danych
    //! \param userPrivilages [out] Uprawnienia u¿ytkownika do danych
    static void getUserPrivilages(int userID, UserPrivilages & userPrivilages);
    //! \param login Login u¿ytkownika w bazie (najprawdopodobniej jego email)
    //! \param password Has³o u¿ytkownika w bazie
    //! \return Identyfikator u¿ytkownika lub -1 je¿eli nie znaleziono
    static const int getUserIDForLoginAndPassword(const std::string & login, const std::string & password);

private:
    //! Identyfikator aktualnie zalogowanego u¿ytkownika
    User user_;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCELOGIN_H__