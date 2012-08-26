/********************************************************************
    created:  2012/02/03
    created:  3:2:2012   17:14
    filename: CommunicationDataSourcePrivilagesVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCEPRIVILAGESVERIFIER_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCEPRIVILAGESVERIFIER_H__

#include <plugins/newCommunication/IDataSourceUser.h>
#include <plugins/newCommunication/Entity.h>
#include <plugins/newCommunication/IDataSourceFilter.h>

#include "DataSourceShallowCopy.h"

//! Klasa veryfikująca prawa dostępu do danych
class PrivilagesVerifier
{
public:

    ////! \param disorder Schorzenie do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli schorzenie pomyślnie przeszło weryfikację
    //static const bool verifyDisorder(const IDisorderAccessor * disorder, const User * user);
    ////! \param disorderOccurence  Wystąpienie schorzenia do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli schorzenie pomyślnie przeszło weryfikację
    //static const bool verifyDisorderOccurence(const IDisorderOccurenceAccessor * disorderOccurence, const User * user);
    ////! \param patient Pacjent do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli pacjent pomyślnie przeszedł weryfikację
    //static const bool verifyPatient(const IPatientAccessor * patient, const User * user);
    ////! \param subject Subject do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli subject pomyślnie przeszedł weryfikację
    //static const bool verifySubject(const ISubjectAccessor * subject, const User * user);
    ////! \param lab Laboratorium do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli laboratorium pomyślnie przeszło weryfikację
    //static const bool verifyLab(const ILabAccessor * lab, const User * user);
    ////! \param session Sesja do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli sesja pomyślnie przeszła weryfikację
    //static const bool verifySession(const ISessionAccessor * session, const User * user);
    ////! \param motion Motion do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli motion pomyślnie przeszedł weryfikację
    //static const bool verifyMotion(const IMotionAccessors * motion, const User * user);
    ////! \param file Plik do weryfikacji
    ////! \param user Użytkownik względem praw którego weryfikujemy dostęp do danych
    ////! \return true jeżeli plik pomyślnie przeszedł weryfikację
    //static const bool verifyFile(const IFileAccessor * file, const User * user);
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCEPRIVILAGESVERIFIER_H__
