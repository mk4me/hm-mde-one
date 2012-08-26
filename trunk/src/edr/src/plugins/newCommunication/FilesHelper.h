//Login i hasło dla nowego testowego użytkownika: applet_user / aplet4Motion

/********************************************************************
    created:  2012/02/23
    created:  23:2:2012   22:36
    filename: FilesHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___FILESHELPER_H__
#define HEADER_GUARD___FILESHELPER_H__

#include <plugins/newCommunication/DataSourceShallowCopy.h>
#include "DataSourceStatus.h"
#include "DataSourceStatusManager.h"

//! Klasa pomocnicza przy zbieraniu plików dla poszczególnych poziomów hierarchi danych i filtorawaniu ich wg statusu
class FilesHelper
{
public:

    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param shallowCopy Płytka kopia danych
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getDisorderFiles(int disorderID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param shallowCopy Płytka kopia danych
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getPatientFiles(int patientID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param shallowCopy Płytka kopia danych
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getSubjectFiles(int subjectID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param shallowCopy Płytka kopia danych
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getSessionFiles(int sessionID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
	//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
	//! \param shallowCopy Płytka kopia danych
	//! \param files [out] Zbiór plików związanych z chorobą
	static void getSessionSpecificFiles(int sessionID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param shallowCopy Płytka kopia danych
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getMotionFiles(int motionID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);

    //! \param inFiles Zbiór plików które chcemy filtrować
    //! \param storage Stan składowania pliku względem którego filtrujemy pliki
    //! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
	//! \param filesStatusManager Manager statusu plików
    static void filterFiles(const std::set<int> & inFiles, communication::DataStorage storage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
    //! \param inFiles Zbiór plików które chcemy filtrować
    //! \param storage Stan załadowania pliku względem którego filtrujemy pliki
    //! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
	//! \param filesStatusManager Manager statusu plików
    static void filterFiles(const std::set<int> & inFiles, communication::DataUsage usage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
    //! \param inFiles Zbiór plików które chcemy filtrować
    //! \param status Stan względem którego filtrujemy pliki
    //! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
	//! \param filesStatusManager Manager statusu plików
    static void filterFiles(const std::set<int> & inFiles, const DataStatus & status, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
	//! \param inFiles Zbiór plików które chcemy filtrować
	//! \param filesExtensions Rozszerzenia plików względem których filtrujemy pliki
	//! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
	//! \param filesStatusManager Manager statusu plików
	static void filterFiles(const std::set<int> & inFiles, const std::set<std::string> & filesExtensions, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
	//! \param inFiles Zbiór plików które chcemy filtrować
	//! \param filesExtension Rozszerzenie plików względem którego filtrujemy pliki
	//! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
	//! \param filesStatusManager Manager statusu plików
	static void filterFiles(const std::set<int> & inFiles, const std::string & filesExtension, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);

    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param shallowCopy Płytka kopia danych
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getFiles(const webservices::MedicalShallowCopy::Disorder * disord, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getFiles(const webservices::MedicalShallowCopy::Patient * patient, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getFiles(const webservices::MotionShallowCopy::Performer * subject, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getFiles(const webservices::MotionShallowCopy::Session * session, std::set<int> & files);
	//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
	//! \param files [out] Zbiór plików związanych z chorobą
	static void getSpecificFiles(const webservices::MotionShallowCopy::Session * session, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
    //! \param files [out] Zbiór plików związanych z chorobą
    static void getFiles(const webservices::MotionShallowCopy::Trial * motion, std::set<int> & files);

	//! \param files Zbiór plików których sumaryczny rozmiar chcemy poznać
	//! \param shallowCopy Płytka kopia bazy danych
	//! \return Sumaryczny rozmiar plików
	static unsigned long long filesSize(const std::set<int> & filesIDs, const communication::ShallowCopy & shallowCopy);

	//! \param file Plik którego rozmiar chcemy poznać
	//! \param shallowCopy Płytka kopia bazy danych
	//! \return Rozmiar pliku
	static unsigned long long fileSize(const int fileID, const communication::ShallowCopy & shallowCopy);
};

#endif	//	HEADER_GUARD___FILESHELPER_H__
