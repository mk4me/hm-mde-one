/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   20:49
    filename: IDataSourceShallowCopy.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCESHALLOWCOPY_H__
#define HEADER_GUARD___IDATASOURCESHALLOWCOPY_H__

#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/any.hpp>

namespace communication
{

//! Typ wartosci pochodz¹cych z bazy danych
//enum ValueType {
//    VTBOOL,       //! Wartoœæ prawda/fa³sz (bool)                                 tekst: "true", "false"
//    VTCHAR,       //! Znak (char)                                                 tekst: ASCII
//    VTINT,        //! Wartoœc sta³oprzecinkowa   (int)                            tekst: wartoœc numeryczna
//    VTFLOAT,      //! Wartoœæ zmiennoprzecinkowa pojedynczej precyzji (float)     tekst: wartoœc numeryczna
//    VTDOUBLE,     //! Wartoœæ zmiennoprzecinkowa podwójnej precyzji (double)      tekst: wartoœc numeryczna
//    VTSTRING,     //! Tekst (std::string)                                         tekst: ta sama wartoœæ
//    VTDATE,       //! Data (boost::date_time::gregorian)                          tekst: DD-MM-RRRR
//    VTTIME,       //! Czas (boost::date_time::gregorian)                          tekst: HH:MM:SS:UUUU
//    VTDATETIME,   //! Data i czas (boost::date_time::gregorian)                   tekst: DD-MM-RRRR HH:MM:SS:UUUU
//    VTPHOTO       //! Zdjêcie (QIcon)                                             tekst: "Photo"
//};

////! Struktura opisuj¹ca pole danych
//struct FieldDescriptor
//{
//    //! Nazwa pola
//    std::string name;
//    //! Opis pola
//    std::string description;
//    //! Typ wartoœci pola
//    ValueType type;
//    //! Jednostka (jeœli wystepuje i dotyczy wartoœci, w przeciwnym razie nullptr)
//    const std::string * unit;
//};
//
//struct IndentifiableFieldDescriptor
//{
//    //! Identyfikator pola
//    int id;
//    FieldDescriptor * fieldDescriptor;
//};
//
////! Klasa przechowuj¹ca rózne wartoœci opisane w wyliczeniu ValueType
//class ValueAccessor
//{
//public:
//    //! KOnstruktor
//    //! \param value Wartoœæ przechowywana
//    //! \param valueType typ wartoœci
//    //! \param valueAsString 
//    ValueAccessor(const boost::any & value, ValueType valueType, const std::string & valueAsString);
//
//    //! \return Wartoœæ jako napis
//    const std::string & valueAsString() const;
//    //! \return Wartoœæ
//    const boost::any & value() const;
//    //! \return Typ wartoœci
//    const ValueType type() const;
//
//private:
//    //! Wartoœæ
//    boost::any value_;
//    //! Typ wartoœci
//    ValueType type_;
//    //! Wartoœæ jako napis
//    std::string valueAsString_;
//};
//
////! Wartoœæ danych zwracana z akcesorów - opisuje typ danych oraz ich wartoœc w formie napisu
////typedef std::pair<ValueType, ValueAccessor> Value;
//typedef ValueAccessor Value;
//
////! Interfejs akcesora do pól danych wraz z ich opisem
//class IFieldAccessor
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IFieldAccessor() {}
//    //! Akcesor do danych z weryfikacj¹ praw u¿ytkownika do tych danych
//    //! \param fieldID Identyfikator pola do odczytania
//    //! \param shallowCopy P³ytka kopia bazy danych
//    virtual const Value & field(int fieldID) const = 0;
//    //! \param idx Indeks opisu pola od 0 do fieldsCount() - 1
//    //! \return Opis pola danych
//    virtual const IndentifiableFieldDescriptor & fieldsDescriptor(int idx) const = 0;
//    //! \return Iloœæ dostêpnych pól danych
//    virtual const int fieldsCount() const = 0;
//};
//
////! Akcesor pól identyfikowalny za pomoc¹ unikalnego ID
//class IIdentifiableFieldAccessor : public IFieldAccessor
//{
//public:
//    //! Destruktor wirtualny
//    virtual ~IIdentifiableFieldAccessor() {}
//
//    //! \return Unikalny identyfikator akcesora
//    virtual const int id() const = 0;
//};
//
////! Interfejs akcesora do atrybutów antropometrycznych
//class IAntropometricAccessor : public IIdentifiableFieldAccessor
//{
//public:
//    //! Pola danych atrybutu antropometrycznego
//    enum Field {
//        NAME,       //! Nazwa atrybutu
//        VALUE,      //! Wartoœæ atrybutu
//        DESCRIPTION //! Opis atrybutu
//    };
//
//public:
//    //! Wirtualny destruktor
//    virtual ~IAntropometricAccessor() {}
//};
//
//class IAttributesAccessor
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IAttributesAccessor() {}
//
//    //! \param attributeID Identyfikator atrybutu
//    //! \return Atrybut zadanego identyfikatora
//    virtual const IFieldAccessor * attribute(int attributeID) const = 0;
//    //! \param idx Indeks atrybutu którego opisu szukamy z przedzia³u 0 - (attributesCount() - 1)
//    //! \return Opis atrybutu którego szukamy
//    virtual const IndentifiableFieldDescriptor & attributeDescriptor(int idx) const = 0;
//    //! \return Iloœæ dostêpnych atrybutów
//    virtual const int attributesCount() const = 0;
//};
//
////! Interfejs akcesora atrybutów antropometrycznych
//class IAntropometricAttributes : public IAttributesAccessor
//{
//public:
//    //! Atrybuty antropometryczne
//    enum Attribute {
//        BodyMass,           //! Masa cia³a
//        Height,             //! Wysokoœæ
//        InterAsisDistance,  //! ?
//        LeftLegLength,      //! D³ugoœæ lewej nogi
//        RightLegLenght,     //! D³ugoœc prawej nogi
//        LeftKneeWidth,      //! Szerokoœæ lewego kolana
//        RightKneeWidth,     //! Szerokoœæ prawego kolana
//        LeftAnkleWidth,     //! Szerokoœæ lewej kostki
//        RightAnkleWidth,    //! Szerokoœæ prawej kostki
//        LeftCircuitThigh,   //! ?
//        RightCircuitThight, //! ?
//        LeftCircuitShank,   //! ?
//        RightCircuitShank,  //! ?
//        LeftShoulderOffset, //! ?
//        RightShoulderOffset,//! ?
//        LeftElbowWidth,     //! Szerokoœæ lewego ³okcia
//        RightElbowWidth,    //! Szerokoœæ prawego ³okcia
//        LeftWristWidth,     //! Szerokoœæ lewego nadgarstka
//        RightWristWidth,    //! Szerokoœæ prawego nadgarstka
//        LeftWristThickness, //! Wysokoœæ lewego nadgarstka
//        RightWristThickness,//! Wysokoœæ prawego nadgarstka
//        LeftHandWidth,      //! Szerokoœæ lewej rêki
//        RightHandWidth,     //! Szerokoœc prawej rêki
//        LeftHandThickness,  //! ?
//        RightHandThickness  //! ?
//    };
//
//public:
//    //! Wirtualny destruktor
//    virtual ~IAntropometricAttributes() {}
//
//    //! \param attributeID Identyfikator atrybutu
//    //! \return Atrybut zadanego identyfikatora
//    virtual const IAntropometricAccessor * attribute(int attributeID) const = 0;
//};
//
//class IDisorderAccessor;
//class IDisorderOccurenceAccessor;
//class IPatientAccessor;
//class ISubjectAccessor;
//class ISessionAccessor;
//class IMotionAccessor;
//class IFileAccessor;
//class IFiles;
//
////! Interfejs akcesora do danych plików z uwzglêdnieniem praw u¿ytkownika do danych
//class IFileAccessor : public IIdentifiableFieldAccessor
//{
//public:
//    //! Pola informacji o plikach
//    enum Field {
//        NAME,       //! Nazwa pliku
//        SIZE,       //! Rozmiar pliku
//        DESCRIPTION //! Opis pliku
//    };
//
//public:
//    //! Wirtualny destruktor
//    virtual ~IFileAccessor() {}
//    //! \return W³aœciciel pliku w hierarchi - jeden z podstawowych akcesorów obiektów które dziedzicza po IFiles
//    virtual const IFiles * owner() const = 0;
//};
//
////! Klasa bazowa elementów agreguj¹cych pliki
//class IFiles
//{
//public:
//    //! Destruktor wirtualny
//    virtual ~IFiles() {}
//    //! \return Iloœæ plików
//    virtual const int filesCount() const = 0;
//    //! \param idx Indeks pliku który chemy pobraæ z przedzia³u 0 - (filesCount() - 1)
//    virtual const IFileAccessor * file(int idx) const = 0;
//};
//
////! Interfejs agreguj¹cy wyst¹pienia schorzeñ
//class IDisorderOccurences
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IDisorderOccurences() {}
//    //! \return Ilosæ wyst¹pieñ danego schorzenia
//    virtual const int disorderOccurencesCount() const = 0;
//    //! \param idx Indeks wystapienia schorzenia z przedzia³u 0 - (disorderOccurencesCount() - 1)
//    //! \return Wyst¹pienie schorzenia
//    virtual const IDisorderOccurenceAccessor * disorderOccurence(int idx) const = 0;
//};
//
////! Interfejs akcesora do danych schorzenia z uwzglêdnieniem praw u¿ytkownika do danych
//class IDisorderAccessor : public IIdentifiableFieldAccessor, public IDisorderOccurences
//{
//public:
//    //! Pola informacji o schorzeniu
//    enum Field {
//        NAME       //! Nazwa schorzenia
//    };
//
//public:
//    //! \return Iloœæ pacjentów z tym schorzeniem
//    virtual const int patientsCount() const = 0;
//    //! \param idx Indeks pacjenta z przedzia³u 0 - (patientsCount() - 1)
//    //! \return Pacjenc o zadanym indeksie
//    virtual const IPatientAccessor * patient(int idx) const = 0;
//
//    //! Destruktor wirtualny
//    virtual ~IDisorderAccessor() {}
//};
//
////! Interfejs akcesora do danych wyst¹pienia schorzenia z uwzglêdnieniem praw u¿ytkownika do danych
//class IDisorderOccurenceAccessor : public IIdentifiableFieldAccessor
//{
//public:
//    //! Pola informacji o wyst¹pieniu schorzeniu
//    enum Field {
//        FOCUS,          //! Istotnoœæ schorzenia w badaniu
//        DIAGNOSIS_DATE, //! Data diagnozy schorzenia
//        COMMENTS        //! Komentarz
//    };
//
//public:
//    //! \return Pacjent którego dotyczy to schorzenie
//    virtual const IPatientAccessor * patient() const = 0;
//    //! \return Schorzenie którego dotyczy ten opis wyst¹pienia schorzenia
//    virtual const IDisorderAccessor * disorder() const = 0;
//
//    //! Destruktor wirtualny
//    virtual ~IDisorderOccurenceAccessor() {}
//};
//
////! Interfejs akcesora do danych pacjenta z uwzglêdnieniem praw u¿ytkownika do danych
//class IPatientAccessor : public IIdentifiableFieldAccessor, public IFiles, public IDisorderOccurences
//{
//public:
//    //! Pola informacji o pacjencie
//    enum Field {
//        NAME,       //! Nazwa pacjenta
//        SURNAME,    //! Nazwisko pacjenta
//        BIRTHDATE,  //! Data urodzenia pacjenta
//        GENDER,     //! P³eæ pacjenta
//        PHOTO       //! Zdjêcie pacjenta
//    };
//
//public:
//    //! \return Subject i ruchy zwi¹zane z danym pacjentem
//    virtual const ISubjectAccessor * subject() const = 0;
//    //! Destruktor wirtualny
//    virtual ~IPatientAccessor() {}
//};
//
////! Forward declaration interfejsu akcesora do sesji
//class ISessionAccessor;
//
//class ISessions
//{
//public:
//    //! Destruktor wirtualny
//    virtual ~ISessions() {}
//    //! \return Iloœc sesji subjecta
//    virtual const int sessionsCount() const = 0;
//    //! \param idx Indeks sesji z zakresu 0 - (sessionsCount() -1)
//    //! \return Akcesor sesji
//    virtual const ISessionAccessor * session(int idx) const = 0;
//};
//
////! Interfejs akcesora do danych subjecta z uwzglêdnieniem praw u¿ytkownika do danych
//class ISubjectAccessor : public IIdentifiableFieldAccessor, public ISessions, public IAttributesAccessor
//{
//public:
//    //! Pola informacji o subject
//    enum Field {
//        NAME    //! Nazwa subjecta
//    };
//
//public:
//    //! \return Pacjent skojarzony z danym subjectem
//    virtual const IPatientAccessor * patient() const = 0;
//    //! Destruktor wirtualny
//    virtual ~ISubjectAccessor() {}
//};
//
////! Forward declaration interfejsu akcesora do motionów
//class IMotionAccessor;
//
////! Interfejs akcesora do danych laboratorium z uwzglêdnieniem praw u¿ytkownika do danych
//class ILabAccessor : public IIdentifiableFieldAccessor, public ISessions
//{
//public:
//    //! Pola danych laboratorium
//    enum Field {
//        NAME    //! Nazwa laboratorium
//    };
//
//public:
//    //! Destruktor wirtualny
//    virtual ~ILabAccessor() {}
//};
//
////! Interfejs akcesora do danych sesji z uwzglêdnieniem praw u¿ytkownika do danych
//class ISessionAccessor : public IIdentifiableFieldAccessor, public IFiles, public IAntropometricAttributes
//{
//public:
//    //! Pola informacji o sesji
//    enum Field {
//        NAME,           //! Nazwa sesji
//        DATE,           //! Data przeprowadzenia sesji
//        OWNER,          //! Identyfikator w³aœciciela sesji (zleceniodawcy)
//        MOTION_KIND,    //! Typ ruchu wykonywanego w sesji
//        TAGS,           //! Tagi sesji
//        DESCRIPTION     //! Opis sesji
//    };
//
//public:
//    //! \return Subject którego dotyczy ta sesja
//    virtual const ISubjectAccessor * subject() const = 0;
//    //! \return Laboratorium gdzie przeprowadzono sesjê
//    virtual const ILabAccessor * lab() const = 0;
//    //! \return Iloœæ ruchów subjecta
//    virtual const int motionsCount() const = 0;
//    //! \param idx Indeks ruchu z przedzia³u 0 - (motionsCount() - 1)
//    //! \return Akcesor ruchu
//    virtual const IMotionAccessor * motion(int idx) const = 0;
//    //! Destruktor wirtualny
//    virtual ~ISessionAccessor() {}
//};
//
////! Interfejs akcesora do danych motiona z uwzglêdnieniem praw u¿ytkownika do danych
//class IMotionAccessors : public IIdentifiableFieldAccessor, public IFiles, public IAttributesAccessor
//{
//public:
//    //! Pola informacji o motion
//    enum Field {
//        NAME,       //! Nazwa motion
//        DESCRIPTION //! Opis motion
//    };
//
//public:
//    //! \return Sesja której dotyczy ten motion
//    virtual const ISessionAccessor * session() const = 0;
//    //! Destruktor wirtualny
//    virtual ~IMotionAccessors() {}
//};
//
////! Mapy obiektów danych ruchu - ID <-> obiekt
//typedef std::map<int, IFileAccessor *> Files;
//typedef std::map<int, IMotionAccessor *> Motions;
//typedef std::map<int, ISessionAccessor *> Sessions;
//typedef std::map<int, ISubjectAccessor *> Subjects;
//typedef std::map<int, ILabAccessor *> Labs;

////! P³ytka kopia bazy danych ruchu uwzglêdniaj¹ca ju¿ prawa u¿ytkownika do danych
//struct MotionShallowCopy
//{
//public:
//    //! Laboratoria
//    Labs labs;
//    //! Subjecty
//    Subjects subjects;
//    //! Sesje subjectów
//    Sessions sessions;
//    //! Nagrania ruchów
//    Motions motions;
//    //! Pliki
//    Files files;
//
//    //! Destruktor
//    ~MotionShallowCopy();
//};
//
////! Mapy obiektów danych medycznych - ID <-> obiekt
//typedef std::map<int, IDisorderAccessor *> Disorders;
//typedef std::map<int, IDisorderOccurenceAccessor *> DisordersOccurences;
//typedef std::map<int, IPatientAccessor *> Patients;
//
////! P³ytka kopia bazy danych medycznych uwzglêdniaj¹ca ju¿ prawa u¿ytkownika do danych
//struct MedicalShallowCopy
//{
//public:
//    //! Schorzenia
//    Disorders disorders;
//    //! Wyst¹pienia schorzeñ
//    DisordersOccurences occurences;
//    //! Pacjenci
//    Patients patients;
//    //! Destruktor
//    ~MedicalShallowCopy();
//};
//
////! P³ytka kopia bazy danych uwzglêdniaj¹ca ju¿ prawa u¿ytkownika do danych
//struct ShallowCopy {
//    MotionShallowCopy motionShallowCopy;
//    MedicalShallowCopy medicalShallowCopy;
//};

}

#endif	//	HEADER_GUARD___IDATASOURCESHALLOWCOPY_H__
