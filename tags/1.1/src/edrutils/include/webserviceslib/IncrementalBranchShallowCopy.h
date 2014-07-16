/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   16:01
	filename: 	IncrementalBranchShallowCopy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_WEBSERVICES__INCREMENTALBRANCHSHALLOWCOPY_H__
#define HEADER_GUARD_WEBSERVICES__INCREMENTALBRANCHSHALLOWCOPY_H__

#include <webserviceslib/ShallowCopy.h>
#include <vector>

namespace webservices {

struct IncrementalBranchShallowCopy
{
    struct File;
    struct Trial;
    struct Session;
    struct Performer;
    struct PerformerConf;
    struct GroupAssigment;
    struct ShallowCopy;

    //! Mapa atrybut -> warto��
    typedef std::map<std::string, std::string> Attrs;

    //! Plik
    struct File
    {
        //! \return Czy plik nale�y do sesji
        const bool isSessionFile() const
        {
            return trialID < 0;
        }

        int trialID;					//! Pr�ba pomiarowa
        int sessionID;				//! Sesja
        int fileID;						//! Identyfikator
        std::string fileName;			//! Nazwa pliku (nazwa + rozszerzenie)
        std::string fileDescription;	//! Opis pliku
        std::string subdirPath;			//! Katalog pliku po stronie bazy danych
        FileSize fileSize;				//! Rozmiar pliku (w bajtach)

        File() : trialID(-1), sessionID(-1), fileID(-1) {}
    };
    //! Mapa ID pliku -> plik
    typedef std::vector<File> Files;

    //! Pr�ba pomiarowa
    struct Trial
    {
        int sessionID;				//! Sesja
        Attrs attrs;					//! Atrybuty
        Files modifiedFiles;		    //! Pliki zmodyfikowane
        Files addedFiles;               //! Pliki dodane
        int trialID;					//! Identyfikator            
        std::string trialName;			//! Nazwa pr�by pomiarowej
        std::string trialDescription;	//! Opis pr�by pomiarowej

        Trial() : sessionID(-1), trialID(-1) {}
    };
    typedef std::vector<Trial> Trials;
    
    //! Sesje
    struct Session
    {
        int performerConfID;		//! Konfiguracja aktora
        int groupAssigmentID;	//! Przynale�no�c do grupy
        Trials trials;						//! Pr�by pomiarowe
        Attrs attrs;						//! Atrybuty
        int sessionID;						//! ID sesji
        int userID;							//! ID u�ytkownika (w�a�ciciel danych)
        int labID;							//! ID laboratorium gdzie nagrywano sesj�
        std::string motionKind;				//! Typ ruchu
        DateTime sessionDate;				//! Data sesji
        std::string sessionName;			//! Nazwa sesji
        std::string tags;					//! Tagi
        std::string sessionDescription;		//! Opis sesji
        Files files;						//! Pliki sesji
        std::string emgConf;				//! Konfiguracja EMG
        Session() : performerConfID(-1), groupAssigmentID(-1), sessionID(-1), userID(-1),labID (-1) {}
    };

    //! Mapa ID sesji -> sesja
    typedef std::vector<Session> Sessions;

    //! Grupa sesji
    struct GroupAssigment
    {
        int sessionGroupID;		//! Identyfikator
        Sessions sessions;		//! Sesje
        GroupAssigment() : sessionGroupID(-1) {}
    };

    //! Mapa ID grupy sesji -> grupa sesji
    typedef std::vector<GroupAssigment> GroupAssigments;

    //! Konfiguracja aktora
    struct PerformerConf
    {
        int performerConfID;	//! Identyfikator
        Attrs attrs;			//! Atrybuty            
        int performerID;	//! Aktor            
        int sessionID;		//! Sesja
        PerformerConf() : performerConfID(-1), performerID(-1), sessionID(-1) {}
    };
    //! Mapa ID konfiguracji aktora -> konfiguracja aktora
    typedef std::vector<PerformerConf> PerformerConfs;

    //! Aktor
    struct Performer
    {
        int performerID;						//! Identyfikator
        Attrs attrs;							//! Atrybuty
        PerformerConfs performerConfs;			//! Konfiguracja aktora
        //MedicalShallowCopy::Patient * patient;	//! Odpowiadaj�cy pacjent z bazy medycznej, mo�e by� nullptr
    };


    //! Mapa ID aktora -> aktor
    typedef std::vector<Performer> Performers;

    //! P�ytka kopia bazy danych
    struct ShallowCopy
    {
    public:
        //DateTime timestamp;					//! Data
        Sessions sessions;					//! Sesje
        GroupAssigments groupAssigments;	//! Grupy sesji
        Trials trials;						//! Pr�by pomiarowe
        Performers performers;				//! Aktorzy
        PerformerConfs performerConfs;		//! Konfiguracje aktor�w
        Files files;						//! Pliki

    };
    ShallowCopy added;
    ShallowCopy modified;

};



}
#endif
