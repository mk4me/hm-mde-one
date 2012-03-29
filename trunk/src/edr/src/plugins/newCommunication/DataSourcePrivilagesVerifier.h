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

//! Klasa veryfikuj¹ca prawa dostêpu do danych
class PrivilagesVerifier
{
public:

    ////! \param disorder Schorzenie do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli schorzenie pomyœlnie przesz³o weryfikacjê
    //static const bool verifyDisorder(const IDisorderAccessor * disorder, const User * user);
    ////! \param disorderOccurence  Wyst¹pienie schorzenia do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli schorzenie pomyœlnie przesz³o weryfikacjê
    //static const bool verifyDisorderOccurence(const IDisorderOccurenceAccessor * disorderOccurence, const User * user);
    ////! \param patient Pacjent do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli pacjent pomyœlnie przeszed³ weryfikacjê
    //static const bool verifyPatient(const IPatientAccessor * patient, const User * user);
    ////! \param subject Subject do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli subject pomyœlnie przeszed³ weryfikacjê
    //static const bool verifySubject(const ISubjectAccessor * subject, const User * user);
    ////! \param lab Laboratorium do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli laboratorium pomyœlnie przesz³o weryfikacjê
    //static const bool verifyLab(const ILabAccessor * lab, const User * user);
    ////! \param session Sesja do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli sesja pomyœlnie przesz³a weryfikacjê
    //static const bool verifySession(const ISessionAccessor * session, const User * user);
    ////! \param motion Motion do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli motion pomyœlnie przeszed³ weryfikacjê
    //static const bool verifyMotion(const IMotionAccessors * motion, const User * user);
    ////! \param file Plik do weryfikacji
    ////! \param user U¿ytkownik wzglêdem praw którego weryfikujemy dostêp do danych
    ////! \return true je¿eli plik pomyœlnie przeszed³ weryfikacjê
    //static const bool verifyFile(const IFileAccessor * file, const User * user);
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCEPRIVILAGESVERIFIER_H__