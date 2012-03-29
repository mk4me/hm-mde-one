//Login i has�o dla nowego testowego u�ytkownika: applet_user / aplet4Motion

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

//! Klasa pomocnicza przy zbieraniu plik�w dla poszczeg�lnych poziom�w hierarchi danych i filtorawaniu ich wg statusu
class FilesHelper
{
public:

    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param shallowCopy P�ytka kopia danych
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getDisorderFiles(int disorderID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param shallowCopy P�ytka kopia danych
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getPatientFiles(int patientID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param shallowCopy P�ytka kopia danych
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getSubjectFiles(int subjectID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param shallowCopy P�ytka kopia danych
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getSessionFiles(int sessionID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
	//! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
	//! \param shallowCopy P�ytka kopia danych
	//! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
	static void getSessionSpecificFiles(int sessionID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param shallowCopy P�ytka kopia danych
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getMotionFiles(int motionID, const communication::ShallowCopy & shallowCopy, std::set<int> & files);

    //! \param inFiles Zbi�r plik�w kt�re chcemy filtrowa�
    //! \param storage Stan sk�adowania pliku wzgl�dem kt�rego filtrujemy pliki
    //! \param filteredFiles [out] Wyj�ciowy zbi�r przefiltrowanych plik�w
	//! \param filesStatusManager Manager statusu plik�w
    static void filterFiles(const std::set<int> & inFiles, communication::DataStorage storage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
    //! \param inFiles Zbi�r plik�w kt�re chcemy filtrowa�
    //! \param storage Stan za�adowania pliku wzgl�dem kt�rego filtrujemy pliki
    //! \param filteredFiles [out] Wyj�ciowy zbi�r przefiltrowanych plik�w
	//! \param filesStatusManager Manager statusu plik�w
    static void filterFiles(const std::set<int> & inFiles, communication::DataUsage usage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
    //! \param inFiles Zbi�r plik�w kt�re chcemy filtrowa�
    //! \param status Stan wzgl�dem kt�rego filtrujemy pliki
    //! \param filteredFiles [out] Wyj�ciowy zbi�r przefiltrowanych plik�w
	//! \param filesStatusManager Manager statusu plik�w
    static void filterFiles(const std::set<int> & inFiles, const DataStatus & status, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
	//! \param inFiles Zbi�r plik�w kt�re chcemy filtrowa�
	//! \param filesExtensions Rozszerzenia plik�w wzgl�dem kt�rych filtrujemy pliki
	//! \param filteredFiles [out] Wyj�ciowy zbi�r przefiltrowanych plik�w
	//! \param filesStatusManager Manager statusu plik�w
	static void filterFiles(const std::set<int> & inFiles, const std::set<std::string> & filesExtensions, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
	//! \param inFiles Zbi�r plik�w kt�re chcemy filtrowa�
	//! \param filesExtension Rozszerzenie plik�w wzgl�dem kt�rego filtrujemy pliki
	//! \param filteredFiles [out] Wyj�ciowy zbi�r przefiltrowanych plik�w
	//! \param filesStatusManager Manager statusu plik�w
	static void filterFiles(const std::set<int> & inFiles, const std::string & filesExtension, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);

    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param shallowCopy P�ytka kopia danych
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getFiles(const webservices::MedicalShallowCopy::Disorder * disord, const communication::ShallowCopy & shallowCopy, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getFiles(const webservices::MedicalShallowCopy::Patient * patient, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getFiles(const webservices::MotionShallowCopy::Performer * subject, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getFiles(const webservices::MotionShallowCopy::Session * session, std::set<int> & files);
	//! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
	//! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
	static void getSpecificFiles(const webservices::MotionShallowCopy::Session * session, std::set<int> & files);
    //! \param disorderID Identyfikator schorzenia dla kt�rego chcemy pobra� pliki
    //! \param files [out] Zbi�r plik�w zwi�zanych z chorob�
    static void getFiles(const webservices::MotionShallowCopy::Trial * motion, std::set<int> & files);

	//! \param files Zbi�r plik�w kt�rych sumaryczny rozmiar chcemy pozna�
	//! \param shallowCopy P�ytka kopia bazy danych
	//! \return Sumaryczny rozmiar plik�w
	static unsigned long long filesSize(const std::set<int> & filesIDs, const communication::ShallowCopy & shallowCopy);

	//! \param file Plik kt�rego rozmiar chcemy pozna�
	//! \param shallowCopy P�ytka kopia bazy danych
	//! \return Rozmiar pliku
	static unsigned long long fileSize(const int fileID, const communication::ShallowCopy & shallowCopy);
};

#endif	//	HEADER_GUARD___FILESHELPER_H__
