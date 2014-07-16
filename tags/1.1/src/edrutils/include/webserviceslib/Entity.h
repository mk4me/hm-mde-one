/**
@author Marek Daniluk
@brief Dokument z prostymi typami, najczęściej encjami bazodanowymi powiązanymi z różnymi technologiami.
*/

#ifndef HEADER_GUARD_COMMUNICATION_ENTITY_H__
#define HEADER_GUARD_COMMUNICATION_ENTITY_H__

#include <string>
#include <list>
#include <webserviceslib/DateTime.h>

namespace webservices
{
	//! Obiekty dla zapytań usług webowych w formie XML
	namespace xmlWsdl
	{
		//! Wyliczenie dla typów encji po stronie bazy danych ruchu
		struct Entity
		{
		public:

			//! Typ wyliczeniowy encji
			enum Type {
				Unspecified,				//! Nieokreślono
				Session,					//! Sesja
				Trial,						//! Próba pomiarowa
				File,						//! Plik
				MeasurementConfiguration,	//! Konfiguracja pomiaru
				Performer,					//! Aktor
				PerformerConfiguration,	//! Konfiguracja aktora
			};

			//! \param entity Tekstowa reprezentacja encji
			//! \return Wyliczeniowa reprezentacja encji
			static const Type convert(const std::string & entity);
			//! \param entity Wyliczeniowa reprezentacja encji
			//! \return Tekstowa reprezentacja encji
			static const std::string convert(const Type & entity);
		};

		//! Wyliczenie dla płci
		struct Gender
		{
		public:

			//! Typ wyliczeniowy płci
			enum Type {
				Unspecified,	//! Nieokreślono
				Male,			//! Mężczyzna
				Female			//! Kobieta
			};

			//! \param entity Tekstowa reprezentacja płci
			//! \return Wyliczeniowa reprezentacja płci
			static const Type convert(const std::string & gender);
			//! \param entity Wyliczeniowa reprezentacja płci
			//! \return Tekstowa reprezentacja płci
			static const std::string convert(const Type & gender);
		};

		//! Typy atrybutów
		struct AttributeType
		{
		public:
			//! Typ wyliczeniowy typów atrybutów
			enum Type {
				Unspecified,			//! Nieokreślono (domyślnie tekst)
				Int,					//! Integer
				Decimal,				//! Wartość dzisiętna
				NonNegativeInteger,	//! Nieujemny integer
				NonNegativeDecimal,	//! Nieujemna wartość dzisiętna
				DateTime,				//! Data i czas
				Date,					//! Data
				TimeCode,				//! TODO co to jest?
				Float					//! Wartość zmiennoprzecinkowa
			};

			//! \param entity Tekstowa reprezentacja typu atrybutu
			//! \return Wyliczeniowa reprezentacja typu atrybutu
			static const Type convert(const std::string & attributeType);
			//! \param entity Wyliczeniowa reprezentacja typu atrybutu
			//! \return Tekstowa reprezentacja typu atrybutu
			static const std::string convert(const Type & attributeType);
		};

		//! Typ logiczny
		struct BooleanType
		{
		public:

			//! Typ wyliczeniowy wartosci logicznych
			enum Type {
				Unspecified,	//! Nieokreślono
				True,			//! Prawda
				False			//! Fałsz
			};

			//! \param booleanType Tekstowa reprezentacja typu logicznego
			//! \return Wyliczeniowa reprezentacja typu logicznego
			static const Type convert(const std::string & booleanType);
			//! \param booleanType Wyliczeniowa reprezentacja typu logicznego
			//! \return Tekstowa reprezentacja typu logicznego
			static const std::string convert(const Type & booleanType);
		};

		//! Typ statusu adnotacji dla danych USG
		struct AnnotationStatus
		{
			//! Typ wyliczeniowy wartosci logicznych
			enum Type {
				Unspecified = -1,		//! Nieokreślono
				UnderConstruction = 1,	//! W edycji
				ReadyForReview = 2,		//! Gotowy do weryfikacji
				UnderReview = 3,		//! W weryfikacji
				Approved = 4,			//! Zatwierdzony
				Rejected = 0			//! Odrzucony
			};

			//! \param statusType Tekstowa reprezentacja typu stanu adnotacji
			//! \return Wyliczeniowa reprezentacja typu stanu adnotacji
			static const Type convert(const std::string & statusType);
			//! \param statusType Wyliczeniowa reprezentacja typu stanu adnotacji
			//! \return Tekstowa reprezentacja typu stanu adnotacji
			static const std::string convert(const Type & statusType);
		};

		//! Opis adnotacji dla danych USG
		struct Annotation
		{
			int trialID;					//! Identyfikator triala którego dotyczy adnotacja
			int userID;						//! Identyfikator uzytkownika którego status dotyczy
			AnnotationStatus::Type status;	//! Status
			std::string comment;			//! Komentarz do statusu
			std::string note;				//! Notatka do statusu
		};

		//! Lista adnotacji
		typedef std::list<Annotation> AnnotationsList;
		//! \param xmlResponse Odpowiedź webserwisów
		//! \return Lista adnotacji użytkownika
		const AnnotationsList parseAnnotations(const std::string & xmlResponse);

		//! Dane grupy użytkowników
		struct UserGroup
		{
			int id;				//! Identyfikator grupy
			std::string name;	//! Nazwa grupy
		};

		//! Lista grup użytkowników
		typedef std::list<UserGroup> UserGroupsList;

		//! \param xmlResponse Odpowiedź webserwisów
		//! \return Lista grup użytkowników
		const UserGroupsList parseUserGroups(const std::string & xmlResponse);

		//! Szczegóły użytkownika
		struct UserDetails
		{
			int id;					//! Identyfikator
			std::string login;		//! Login użytkownika
			std::string firstName;	//! Imię
			std::string lastName;	//! Nazwisko
		};

		//! Lista użytkowników
		typedef std::list<UserDetails> UsersList;

		//! \param xmlResponse Odpowiedź webserwisów
		//! \return Lista użytkowników w bazie
		const UsersList parseUsersList(const std::string & xmlResponse);

		//! Użytkownik
		struct User : public UserDetails
		{
			std::string email;	//! Adres email użytkownika
		};

		//! \param xmlResponse Odpowiedź webserwisów
		//! \return Dane użytkownika
		const User parseUser(const std::string & xmlResponse);

		//! Prawa dostępu do sesji
		struct SessionPrivilege
		{
			std::string login;	//! Login użytkownika
			bool canWrite;		//! Czy użytkownik ma prawa zapisu (edycji) sesji
		};

		//! Lista praw dostępu użytkowników do sesji
		typedef std::list<SessionPrivilege> SessionPrivilegeList;

		//! Aktor
		struct PerformerData
		{
			std::string name;		//! Imię aktora
			std::string surname;	//! Nazwisko aktora
		};

		//! Opis atrybutu
		struct Attribute
		{
			std::string name;			//! Nazwa
			std::string value;			//! Wartość
			Entity::Type entity;		//! Typ encji
			std::string attributeGroup;	//! Grupa
			AttributeType::Type type;	//! Typ
		};

		//! Typ listy atrybutów
		typedef std::list<Attribute> Attributes;

		//! Opis grupy atrybutów
		struct AttributeGroupDefinition
		{
			std::string name;				//! Nazwa
			Entity::Type describedEntity;	//! Opisywana encja
		};

		//! Typ listy grup atrybutów
		typedef std::list<AttributeGroupDefinition> AttributeGroupDefinitionList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista definicji atrybutów
		const AttributeGroupDefinitionList parseAttributeGroupDefinitions(const std::string & xmlResponse);

		//! Dane aktora
		struct PerformerDetails
		{
			int id;					//! Identyfikator
			std::string firstName;	//! Imię
			std::string lastName;	//! Nazwisko
		};

		//! Typ listy aktorów
		typedef std::list<PerformerDetails> PerformerList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista aktorów
		const PerformerList parsePerfomers(const std::string & xmlResponse);

		//! Dane aktora z atrybutami
		struct PerformerDetailsWithAttributes
		{
			PerformerDetails performerDetails;	//! Szczegóły aktora
			Attributes attributes;				//! Atrybuty
		};

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Opis aktora z atrybutami
		const PerformerDetailsWithAttributes parsePerformer(const std::string & xmlResponse);

		//! Typ listy ze szczegółami aktorów i ich atrybutami
		typedef std::list<PerformerDetailsWithAttributes> PerformerWithAttributesList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista aktorów z atrybutami
		const PerformerWithAttributesList parsePerfomersWithAttributes(const std::string & xmlResponse);

		//! Dane sesji
		struct SessionDetails
		{
			int id;						//! Identyfikator
			int userID;					//! Właściciel
			int labID;					//! Laboratorium
			std::string motionKind;		//! Typ ruchu
			DateTime dateTime;			//! Czas rejestracji
			std::string description;	//! Opis
			std::string label;			//! Nazwa
		};

		//! Typ listy z sesjami
		typedef std::list<SessionDetails> SessionList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista sesji
		const SessionList parseSessions(const std::string & xmlResponse);

		//! Dane sesji z atrybutami
		struct SessionDetailsWithAttributes
		{
			SessionDetails sessionDetails;	//! Szczegóły sesji
			Attributes attributes;			//! Atrybuty
		};

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Opis sesji z atrybutami
		const SessionDetailsWithAttributes parseSession(const std::string & xmlResponse);

		//! Typ listy sesji
		typedef std::list<SessionDetailsWithAttributes> SessionWithAttributesList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista sesji z atrybutami
		const SessionWithAttributesList parseSessionsWithAttributes(const std::string & xmlResponse);

		//! Dane grupy sesji
		struct SessionGroupDefinition
		{
			int id;				//! Identyfikator
			std::string name;	//! Nazwa
		};

		//! Typ listy grup sesji
		typedef std::list<SessionGroupDefinition> SessionGroupDefinitionList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista grup sesji
		const SessionGroupDefinitionList parseSessionGroups(const std::string & xmlResponse);

		//! Opis typów ruchów
		struct MotionKindDefinition
		{
			int id;				//! Identyfikator
			std::string name;	//! Nazwa
		};

		//! Typ listy typów ruchów
		typedef std::list<MotionKindDefinition> MotionKindDefinitionList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista definicji typów ruchów
		const MotionKindDefinitionList parseMotionKinds(const std::string & xmlResponse);

		//! Dane próby pomiarowej
		struct TrialDetails
		{
			int id;						//! Identyfikator
			int sessionID;				//! Sesja
			std::string description;	//! Opis
		};

		//! Typ listy triali
		typedef std::list<TrialDetails> TrialList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista prób pomiarowych
		const TrialList parseTrials(const std::string & xmlResponse);

		//! Dane próby pomiarowej
		struct TrialDetailsWithAttributes
		{
			TrialDetails trialDetails;	//! Szczegóły próby pomiarowej
			Attributes attributes;		//! Atrybuty
		};

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Opis próby pomiarowej z atrybutami
		const TrialDetailsWithAttributes parseTrial(const std::string & xmlResponse);

		//! Typ listy triali z atrybutami
		typedef std::list<TrialDetailsWithAttributes> TrialsWithAttributesList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista prób pomiarowych z atrybutami
		const TrialsWithAttributesList parseTrialsWithAttributes(const std::string & xmlResponse);

		//! Dane konfiguracji pomiarowej
		struct MeasurementConfDetails
		{
			int id;						//! Identyfikator
			std::string name;			//! Nazwa
			std::string kind;			//! Rodzaj
			std::string description;	//! Opis
		};


		//! Dane konfiguracji pomiarowej
		struct MeasurementConfDetailsWithAttributes
		{
			MeasurementConfDetails measurementConfDetails;	//! Szczegóły konfiguracji pomiarowej
			Attributes attributes;							//! Atrybuty
		};

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Opis konfiguracji pomiarowej z atrybutami
		const MeasurementConfDetailsWithAttributes parseMeasurementConf(const std::string & xmlResponse);

		//! Typ listy z konfiguracjami pomiarowymi
		typedef std::list<MeasurementConfDetailsWithAttributes> MeasurementConfWithAttributesList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista konfiguracji pomiarowych z atrybutami
		const MeasurementConfWithAttributesList parseMeasurementsConfWithAttributes(const std::string & xmlResponse);

		//! Dane konfiguracji aktora
		struct PerformerConfDetails
		{
			int id;					//! Identyfikator
			int sessionID;			//! Sesja
			int performerID;		//! Aktor
		};

		//! Dane konfiguracji aktora
		struct PerformerConfDetailsWithAttributes
		{
			PerformerConfDetails performerConfDetails;	//! Szczegóły konfiguracji aktora
			Attributes attributes;						//! Atrybuty
		};

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Opis konfiguracji aktora z atrybutami
		const PerformerConfDetailsWithAttributes parsePerfomerConf(const std::string & xmlResponse);

		//! Typ listy konfiguracji aktorów
		typedef std::list<PerformerConfDetailsWithAttributes> PerformerConfWithAttributesList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista konfiguracji aktorów z atrybutami
		const PerformerConfWithAttributesList parsePerformersConfWithAttributes(const std::string & xmlResponse);

		//! Dane pliku
		struct FileData
		{
			std::string fileLocation;	//! Ścieżka względna do pliku
			std::string subdirPath;		//! Katalog pliku jeśli był ładowany przez operację grupową
		};

		//! Opis pliku
		struct FileDetails
		{
			int id;						//! Identyfikator
			std::string name;			//! Nazwa
			std::string description;	//! Opis
			std::string subdirPath;		//! Ścieżka
			std::string attributeName;	//! Nazwa atrybutu
		};

		//! Typ listy plików
		typedef std::list<FileDetails> FileList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista plików
		const FileList parseFiles(const std::string & xmlResponse);

		//! Opis pliku z atrybutami
		struct FileDetailsWithAttribute
		{
			FileDetails fileDetails;	//! Szczegóły pliku
			Attributes attributes;		//! Atrybuty
		};

		//! Typ listy plików z atrybutami
		typedef std::list<FileDetailsWithAttribute> FileWithAttributeList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista plików z atrybutami
		const FileWithAttributeList parseFilesWithAttributes(const std::string & xmlResponse);

		//! Typ wartości enum
		typedef std::string EnumValue;

		//! Typ listy wartości enum
		typedef std::list<EnumValue> EnumValueList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista wartości wyliczeń
		const EnumValueList parseEnumValues(const std::string & xmlResponse);

		//! Dane atrybutu
		struct AttributeDefinition
		{
			std::string name;			//! Nazwa
			std::string type;			//! Typ
			int enumerate;				//! Identyfikator
			std::string groupName;		//! Nazwa grupy
			std::string unit;			//! Jednostka
			EnumValueList enumValues;	//! Wartości
		};

		//! Typ listy definicji atrybutów
		typedef std::list<AttributeDefinition> AttributeDefinitionList;

		//! \param xmlResponse Odpowiedź na zapytanie
		//! \return Lista plików z atrybutami
		const AttributeDefinitionList parseAttributesDefinitions(const std::string & xmlResponse);
	}    
}

#endif //HEADER_GUARD_COMMUNICATION_ENTITY_H__
