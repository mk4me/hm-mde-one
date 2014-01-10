/********************************************************************
    created:  2013/11/05
    created:  5:11:2013   10:19
    filename: ShallowCopy.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___SHALLOWCOPY_H__
#define HEADER_GUARD___SHALLOWCOPY_H__

#include <map>
#include <set>
#include <string>
#include <webserviceslib/DateTime.h>
#include <webserviceslib/Entity.h>

namespace webservices
{
	//! Typ opisuj�cy rozmiar pliku ( w bajtach)
	typedef unsigned long long FileSize;

	//! Forward declaration
	namespace MedicalShallowCopy
	{
		struct Patient;
	};

	/**
    Encje bazodanowe z operacji p�ytkiej kopii bazy danych. Schemat MotionShallowCopy.
    */
    namespace MotionShallowCopy
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
                return trial == nullptr;
            }

            Trial * trial;					//! Pr�ba pomiarowa
            Session * session;				//! Sesja
            int fileID;						//! Identyfikator
            std::string fileName;			//! Nazwa pliku (nazwa + rozszerzenie)
            std::string fileDescription;	//! Opis pliku
            std::string subdirPath;			//! Katalog pliku po stronie bazy danych
			FileSize fileSize;				//! Rozmiar pliku (w bajtach)
        };

        //! Mapa ID pliku -> plik
        typedef std::map<int, File*> Files;

		//! Pr�ba pomiarowa
        struct Trial
        {
            Session * session;				//! Sesja
            Attrs attrs;					//! Atrybuty
            Files files;					//! Pliki
            int trialID;					//! Identyfikator            
            std::string trialName;			//! Nazwa pr�by pomiarowej
            std::string trialDescription;	//! Opis pr�by pomiarowej
        };

        //! Mapa ID pr�by pomiarowej -> pr�ba pomiarowa
        typedef std::map<int, Trial*> Trials;

		//! Sesje
        struct Session
        {
            PerformerConf * performerConf;		//! Konfiguracja aktora
			GroupAssigment * groupAssigment;	//! Przynale�no�c do grupy
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
        };

        //! Mapa ID sesji -> sesja
        typedef std::map<int, Session*> Sessions;

		//! Grupa sesji
        struct GroupAssigment
        {
			int sessionGroupID;		//! Identyfikator
            Sessions sessions;		//! Sesje
        };

		//! Mapa ID grupy sesji -> grupa sesji
        typedef std::map<int, GroupAssigment*> GroupAssigments;

		//! Konfiguracja aktora
        struct PerformerConf
        {
			int performerConfID;	//! Identyfikator
            Attrs attrs;			//! Atrybuty            
            Performer * performer;	//! Aktor            
            Session * session;		//! Sesja
        };

        //! Mapa ID konfiguracji aktora -> konfiguracja aktora
        typedef std::map<int, PerformerConf*> PerformerConfs;

		//! Aktor
        struct Performer
        {
			int performerID;						//! Identyfikator
            Attrs attrs;							//! Atrybuty
            PerformerConfs performerConfs;			//! Konfiguracja aktora
            MedicalShallowCopy::Patient * patient;	//! Odpowiadaj�cy pacjent z bazy medycznej, mo�e by� nullptr
        };

        //! Mapa ID aktora -> aktor
        typedef std::map<int, Performer*> Performers;

		//! P�ytka kopia bazy danych
        struct ShallowCopy
        {
        public:
            DateTime timestamp;					//! Data
            Sessions sessions;					//! Sesje
            GroupAssigments groupAssigments;	//! Grupy sesji
            Trials trials;						//! Pr�by pomiarowe
            Performers performers;				//! Aktorzy
            PerformerConfs performerConfs;		//! Konfiguracje aktor�w
            Files files;						//! Pliki

			//! Destruktor
            ~ShallowCopy()
            {
                for(auto it = sessions.begin(); it != sessions.end(); ++it){
                    delete it->second;
                }

                for(auto it = trials.begin(); it != trials.end(); ++it){
                    delete it->second;
                }

                for(auto it = performers.begin(); it != performers.end(); ++it){
                    delete it->second;
                }

                for(auto it = performerConfs.begin(); it != performerConfs.end(); ++it){
                    delete it->second;
                }

				for(auto it = groupAssigments.begin(); it != groupAssigments.end(); ++it){
					delete it->second;
				}

                for(auto it = files.begin(); it != files.end(); ++it){
                    delete it->second;
                }
            }

			//! Domy�lny konstruktor
			ShallowCopy() : timestamp(0) {}
        };
    }

    namespace MedicalShallowCopy
    {
		//! Schorzenie
        struct Disorder
        {
            int disorderID;		//! Identyfikator
            std::string name;	//! Nazwa schorzenia
        };

		//! Mapa ID schorzenia -> schorzenie
        typedef std::map<int, Disorder*> Disorders;

		//! Wystapienie schorzenia
        struct DisorderOccurence
        {
            Disorder * disorder;		//! Szchorzenie
            std::string focus;			//! Poziom istotno�ci schorzenia
            DateTime diagnosisDate;		//! Data diagnozy
            std::string comments;		//! Komentarz do wyst�pienia schorzenia
        };

		//! Mapa ID schorzenia -> wyst�pienia schorzenia
        typedef std::map<int, DisorderOccurence> PatientDisorders;

		//! Pacjent
        struct Patient
        {
            int patientID;								//! Identyfikator
            int motionPerformerID;						//! Identyfikator aktora bazy danych ruchu
            std::string name;							//! Imi�
            std::string surname;						//! Nazwisko
            Date birthDate;							//! Data urodzenia
            xmlWsdl::Gender::Type gender;				//! P�e�
            PatientDisorders disorders;					//! Schorzenia pacjenta
            MotionShallowCopy::Performer * performer;	//! Odpowiadaj�cy aktor z bazy danych ruchu
        };

		//! Mapa ID pacjenta -> pacjent
        typedef std::map<int, Patient*> Patients;

		//! Mapa p�e� -> zbi�r pacjent�w
        typedef std::map<xmlWsdl::Gender::Type, std::set<Patient*> > PatientsByGender;

		//! Mapa ID schorzenia -> zbi�r pacjent�w
        typedef std::map<int, std::set<Patient*> > PatientsByDisorder;

		//! P�ytka kopia
        struct ShallowCopy
        {
            DateTime timestamp;						//! Data
            Disorders disorders;					//! Schorzenia
            Patients patients;						//! Pacjenci
            PatientsByGender patientsByGender;		//! Pacjenci wg p�ci
            PatientsByDisorder patientsByDisorder;	//! Pacjenci wg schorze�

			//! Destruktor
            ~ShallowCopy()
            {
                for(auto it = disorders.begin(); it !=disorders.end(); ++it){
                    delete it->second;
                }

                for(auto it = patients.begin(); it != patients.end(); ++it){
                    delete it->second;
                }
            }

			//! Konstruktor domy�lny
			ShallowCopy() : timestamp(0) {}
        };
    }

	/**
    Encje bazodanowe z operacji p�ytkiej kopii bazy danych. Schemat MotionMetaData.
    */
    namespace MotionMetaData
    {
		//! Grupa sesji
        struct SessionGroup
        {
			int sessionGroupID;				//! Identyfikator
			std::string sessionGroupName;	//! Nazwa grupy sesji
        };

		//! Typ mapy ID grupy sesji -> grupa sesji
        typedef std::map<int, SessionGroup> SessionGroups;

		//! Typ ruchu
        struct MotionKind
        {
            std::string motionKindName;		//! Nazwa typu ruchu
        };

		//! Agregat typ�w ruch�w
        typedef std::list<MotionKind> MotionKinds;

		//! Laboratorium
        struct Lab
        {
            int labID;				//! Identyfikator
            std::string labName;	//! Nazwa laboratorium
        };

		//! Mapa ID laboratorium -> laboratorium
        typedef std::map<int, Lab> Labs;

		//! Atrybut
        struct Attribute
        {
            std::string attributeName;					//! Nazwa atrybutu (identyfikator, klucz)
            xmlWsdl::AttributeType::Type attributeType;	//! Typ atrybutu
			std::string unit;							//! Jednostka
        };

		//! Mapa nazwa atrybutu -> atrybut
        typedef std::map<std::string, Attribute> Attributes;

		//! Grupa atrybut�w
        struct AttributeGroup
        {
			int attributeGroupID;					//! Identyfikator grupy atrybut�w
            Attributes attributes;					//! Atrybuty tworz�ce grup�
            std::string attributeGroupName;			//! Nazwa grupy atrybut�w
            xmlWsdl::Entity::Type describedEntity;	//! Opisywana encja
        };

		//! Agregat grup atrybut�w
        typedef std::map<int, AttributeGroup> AttributeGroups;

		//! Metadane
        struct MetaData
        {
            DateTime timestamp;					//! Data
            SessionGroups sessionGroups;		//! Grupy sesji
            MotionKinds motionKinds;			//! Typy ruch�w
            Labs labs;							//! Laboratoria
            AttributeGroups attributeGroups;	//! Grupy atrybut�w

			//! Domy�lny konstruktor
			MetaData() : timestamp(0) {}
        };
    }

    namespace MedicalMetaData
    {
		//! Typ badania
        struct ExamType
        {
            int examTypeID;		//! Identyfikator
            std::string name;	//! Nazwa typu badania
        };
		
		//! Mapa ID typu badania -> typ badania
        typedef std::map<int, ExamType> ExamTypes;

		//! Typ schorzenia
        struct DisorderType
        {
            int disorderTpeID;	//! Identyfikator
            std::string name;	//! Nazwa schorzenia
        };

		//! Mapa ID typu schorzenia -> typ schorzenia
        typedef std::map<int, DisorderType> DisorderTypes;

		//! Metadane
        struct MetaData
        {
            DateTime timestamp;				//! Data
            ExamTypes examTypes;			//! Typy bada�
            DisorderTypes disorderTypes;	//! Typy schorze�

			//! Domy�lny konstruktor
			MetaData() : timestamp(0) {}
        };
    }

}

#endif	//	HEADER_GUARD___SHALLOWCOPY_H__
