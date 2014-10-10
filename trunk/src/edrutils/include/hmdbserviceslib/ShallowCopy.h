/********************************************************************
created:  2013/11/05
created:  5:11:2013   10:19
filename: ShallowCopy.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_SHALLOWCOPY_H__
#define HEADER_GUARD__HMDBSERVICES_SHALLOWCOPY_H__

#include <map>
#include <set>
#include <string>
#include <hmdbserviceslib/DateTime.h>
#include <hmdbserviceslib/Entity.h>

namespace hmdbServices
{
	//! Typ opisuj¹cy rozmiar pliku ( w bajtach)
	typedef unsigned long long FileSize;

	//! Idektyfikator danych
	typedef int ID;

	//! Forward declaration
	namespace MedicalShallowCopy
	{
		struct Patient;
	};

	/**
	Encje bazodanowe z operacji p³ytkiej kopii bazy danych. Schemat MotionShallowCopy.
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

		//! Mapa atrybut -> wartoœæ
		typedef std::map<std::string, std::string> Attrs;

		//! Plik
		struct File
		{
			//! \return Czy plik nale¿y do sesji
			inline const bool isSessionFile() const
			{
				return trial == nullptr;
			}

			Trial * trial;					//! Próba pomiarowa
			Session * session;				//! Sesja
			ID fileID;						//! Identyfikator
			std::string fileName;			//! Nazwa pliku (nazwa + rozszerzenie)
			std::string fileDescription;	//! Opis pliku
			std::string subdirPath;			//! Katalog pliku po stronie bazy danych
			FileSize fileSize;				//! Rozmiar pliku (w bajtach)
		};

		//! Mapa ID pliku -> plik
		typedef std::map<ID, File*> Files;

		//! Próba pomiarowa
		struct Trial
		{
			Session * session;				//! Sesja
			Attrs attrs;					//! Atrybuty
			Files files;					//! Pliki
			ID trialID;					//! Identyfikator
			std::string trialName;			//! Nazwa próby pomiarowej
			std::string trialDescription;	//! Opis próby pomiarowej
		};

		//! Mapa ID próby pomiarowej -> próba pomiarowa
		typedef std::map<ID, Trial*> Trials;

		//! Sesje
		struct Session
		{
			PerformerConf * performerConf;		//! Konfiguracja aktora
			GroupAssigment * groupAssigment;	//! Przynale¿noœc do grupy
			Trials trials;						//! Próby pomiarowe
			Attrs attrs;						//! Atrybuty
			ID sessionID;						//! ID sesji
			ID userID;							//! ID u¿ytkownika (w³aœciciel danych)
			ID labID;							//! ID laboratorium gdzie nagrywano sesjê
			std::string motionKind;				//! Typ ruchu
			DateTime sessionDate;				//! Data sesji
			std::string sessionName;			//! Nazwa sesji
			std::string tags;					//! Tagi
			std::string sessionDescription;		//! Opis sesji
			Files files;						//! Pliki sesji
			std::string emgConf;				//! Konfiguracja EMG
		};

		//! Mapa ID sesji -> sesja
		typedef std::map<ID, Session*> Sessions;

		//! Grupa sesji
		struct GroupAssigment
		{
			ID sessionGroupID;		//! Identyfikator
			Sessions sessions;		//! Sesje
		};

		//! Mapa ID grupy sesji -> grupa sesji
		typedef std::map<ID, GroupAssigment*> GroupAssigments;

		//! Konfiguracja aktora
		struct PerformerConf
		{
			ID performerConfID;	//! Identyfikator
			Attrs attrs;			//! Atrybuty
			Performer * performer;	//! Aktor
			Session * session;		//! Sesja
		};

		//! Mapa ID konfiguracji aktora -> konfiguracja aktora
		typedef std::map<ID, PerformerConf*> PerformerConfs;

		//! Aktor
		struct Performer
		{
			ID performerID;						//! Identyfikator
			Attrs attrs;							//! Atrybuty
			PerformerConfs performerConfs;			//! Konfiguracja aktora
			MedicalShallowCopy::Patient * patient;	//! Odpowiadaj¹cy pacjent z bazy medycznej, mo¿e byæ nullptr
		};

		//! Mapa ID aktora -> aktor
		typedef std::map<ID, Performer*> Performers;

		//! P³ytka kopia bazy danych
		struct ShallowCopy
		{
		public:
			DateTime timestamp;					//! Data
			Sessions sessions;					//! Sesje
			GroupAssigments groupAssigments;	//! Grupy sesji
			Trials trials;						//! Próby pomiarowe
			Performers performers;				//! Aktorzy
			PerformerConfs performerConfs;		//! Konfiguracje aktorów
			Files files;						//! Pliki

			//! Destruktor
			~ShallowCopy()
			{
				for (auto it = sessions.begin(); it != sessions.end(); ++it){
					delete it->second;
				}

				for (auto it = trials.begin(); it != trials.end(); ++it){
					delete it->second;
				}

				for (auto it = performers.begin(); it != performers.end(); ++it){
					delete it->second;
				}

				for (auto it = performerConfs.begin(); it != performerConfs.end(); ++it){
					delete it->second;
				}

				for (auto it = groupAssigments.begin(); it != groupAssigments.end(); ++it){
					delete it->second;
				}

				for (auto it = files.begin(); it != files.end(); ++it){
					delete it->second;
				}
			}

			//! Domyœlny konstruktor
			ShallowCopy() : timestamp(0) {}
			ShallowCopy(const ShallowCopy & other) = delete;
			ShallowCopy(ShallowCopy && other) : timestamp(std::move(other.timestamp)), sessions(std::move(other.sessions)),
				groupAssigments(std::move(other.groupAssigments)), trials(std::move(other.trials)),
				performers(std::move(other.performers)), performerConfs(std::move(other.performerConfs)),
				files(std::move(other.files)) {}
		};
	}

	namespace MedicalShallowCopy
	{
		//! Schorzenie
		struct Disorder
		{
			ID disorderID;		//! Identyfikator
			std::string name;	//! Nazwa schorzenia
		};

		//! Mapa ID schorzenia -> schorzenie
		typedef std::map<ID, Disorder*> Disorders;

		//! Wystapienie schorzenia
		struct DisorderOccurence
		{
			Disorder * disorder;		//! Szchorzenie
			std::string focus;			//! Poziom istotnoœci schorzenia
			DateTime diagnosisDate;		//! Data diagnozy
			std::string comments;		//! Komentarz do wyst¹pienia schorzenia
		};

		//! Mapa ID schorzenia -> wyst¹pienia schorzenia
		typedef std::map<ID, DisorderOccurence> PatientDisorders;

		//! Pacjent
		struct Patient
		{
			ID patientID;								//! Identyfikator
			ID motionPerformerID;						//! Identyfikator aktora bazy danych ruchu
			std::string name;							//! Imiê
			std::string surname;						//! Nazwisko
			Date birthDate;							//! Data urodzenia
			xmlWsdl::Gender::Type gender;				//! P³eæ
			PatientDisorders disorders;					//! Schorzenia pacjenta
			MotionShallowCopy::Performer * performer;	//! Odpowiadaj¹cy aktor z bazy danych ruchu
		};

		//! Mapa ID pacjenta -> pacjent
		typedef std::map<ID, Patient*> Patients;

		//! P³ytka kopia
		struct ShallowCopy
		{
			DateTime timestamp;						//! Data
			Disorders disorders;					//! Schorzenia
			Patients patients;						//! Pacjenci

			//! Destruktor
			~ShallowCopy()
			{
				for (auto it = disorders.begin(); it != disorders.end(); ++it){
					delete it->second;
				}

				for (auto it = patients.begin(); it != patients.end(); ++it){
					delete it->second;
				}
			}

			//! Konstruktor domyœlny
			ShallowCopy() : timestamp(0) {}
			ShallowCopy(const ShallowCopy & other) = delete;
			ShallowCopy(ShallowCopy && other) : timestamp(std::move(other.timestamp)),
				disorders(std::move(other.disorders)), patients(std::move(other.patients)) {}
		};
	}

	/**
	Encje bazodanowe z operacji p³ytkiej kopii bazy danych. Schemat MotionMetaData.
	*/
	namespace MotionMetaData
	{
		//! Grupa sesji
		struct SessionGroup
		{
			ID sessionGroupID;				//! Identyfikator
			std::string sessionGroupName;	//! Nazwa grupy sesji
		};

		//! Typ mapy ID grupy sesji -> grupa sesji
		typedef std::map<ID, SessionGroup> SessionGroups;

		//! Typ ruchu
		struct MotionKind
		{
			std::string motionKindName;		//! Nazwa typu ruchu
		};

		//! Agregat typów ruchów
		typedef std::list<MotionKind> MotionKinds;

		//! Laboratorium
		struct Lab
		{
			int labID;				//! Identyfikator
			std::string labName;	//! Nazwa laboratorium
		};

		//! Mapa ID laboratorium -> laboratorium
		typedef std::map<ID, Lab> Labs;

		//! Atrybut
		struct Attribute
		{
			std::string attributeName;					//! Nazwa atrybutu (identyfikator, klucz)
			xmlWsdl::AttributeType::Type attributeType;	//! Typ atrybutu
			std::string unit;							//! Jednostka
		};

		//! Mapa nazwa atrybutu -> atrybut
		typedef std::map<std::string, Attribute> Attributes;

		//! Grupa atrybutów
		struct AttributeGroup
		{
			ID attributeGroupID;					//! Identyfikator grupy atrybutów
			Attributes attributes;					//! Atrybuty tworz¹ce grupê
			std::string attributeGroupName;			//! Nazwa grupy atrybutów
			xmlWsdl::Entity::Type describedEntity;	//! Opisywana encja
		};

		//! Agregat grup atrybutów
		typedef std::map<ID, AttributeGroup> AttributeGroups;

		//! Metadane
		struct MetaData
		{
			DateTime timestamp;					//! Data
			SessionGroups sessionGroups;		//! Grupy sesji
			MotionKinds motionKinds;			//! Typy ruchów
			Labs labs;							//! Laboratoria
			AttributeGroups attributeGroups;	//! Grupy atrybutów

			//! Domyœlny konstruktor
			MetaData() : timestamp(0) {}
			MetaData(const MetaData & other) = delete;
			MetaData(MetaData && other) : sessionGroups(std::move(other.sessionGroups)),
				motionKinds(std::move(other.motionKinds)), labs(std::move(other.labs)),
				attributeGroups(std::move(other.attributeGroups)) {}
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
			ExamTypes examTypes;			//! Typy badañ
			DisorderTypes disorderTypes;	//! Typy schorzeñ

			//! Domyœlny konstruktor
			MetaData() : timestamp(0) {}
			MetaData(const MetaData & other) = delete;
			MetaData(MetaData && other) : examTypes(std::move(other.examTypes)),
				disorderTypes(std::move(other.disorderTypes)) {}
		};
	}
}

#endif	//	HEADER_GUARD__HMDBSERVICES_SHALLOWCOPY_H__
