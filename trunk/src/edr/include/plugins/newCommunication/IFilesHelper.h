/********************************************************************
    created:  2012/02/17
    created:  17:2:2012   9:05
    filename: IFilesHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILESHELPER_H__
#define HEADER_GUARD___IFILESHELPER_H__

namespace communication
{

//! Interfejs pomocniczy przy operacjach na plikach
class IFilesHelper
{
public:
    //! Wirtualny destruktor
    virtual ~IFilesHelper() {}

    //! \param disorderID Identyfikator schorzenia
    //! \param files [out] Zbi�r plik�w dla zadanego schorzenia
    virtual void getDisorderFiles(int disorderID, std::set<int> & files) const = 0;
    //! \param patientID Identyfikator pacjenta
    //! \param files [out] Zbi�r plik�w dla zadanego pacjenta
    virtual void getPatientFiles(int patientID, std::set<int> & files) const = 0;
    //! \param subjectID Identyfikator subjecta
    //! \param files [out] Zbi�r plik�w dla zadanego subjecta
    virtual void getSubjectFiles(int subjectID, std::set<int> & files) const = 0;
    //! \param sessionID Identyfikator sesji
    //! \param files [out] Zbi�r plik�w dla zadanej sesji
    virtual void getSessionFiles(int sessionID, std::set<int> & files) const = 0;
    //! \param motionID Identyfikator motiona
    //! \param files [out] Zbi�r plik�w dla zadanego motiona
    virtual void getMotionFiles(int motionID, std::set<int> & files) const = 0;

    //! \param src Zbi�r plik�w wejsciowych do przefiltrowania
    //! \param dest [out] Zbi�r przefiltrowanych plik�w za�adowanych do DataManagera
    virtual void filterLoadedFiles(const std::set<int> & src, std::set<int> & dest) const = 0;
    //! \param src Zbi�r plik�w wejsciowych do przefiltrowania
    //! \param dest [out] Zbi�r przefiltrowanych plik�w nieza�adowanych do DataManagera
    virtual void filterUnloadedFiles(const std::set<int> & src, std::set<int> & dest) const = 0;
    //! \param src Zbi�r plik�w wejsciowych do przefiltrowania
    //! \param dest [out] Zbi�r przefiltrowanych plik�w lokalnych
    virtual void filterLocalFiles(const std::set<int> & src, std::set<int> & dest) const = 0;
    //! \param src Zbi�r plik�w wejsciowych do przefiltrowania
    //! \param dest [out] Zbi�r przefiltrowanych plik�w zdalnych
    virtual void filterRemoteFiles(const std::set<int> & src, std::set<int> & dest) const = 0;
};

}

#endif	//	HEADER_GUARD___IFILESHELPER_H__
