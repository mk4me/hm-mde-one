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

//! Typ wartosci pochodzących z bazy danych
//enum ValueType {
//    VTBOOL,       //! Wartość prawda/fałsz (bool)                                 tekst: "true", "false"
//    VTCHAR,       //! Znak (char)                                                 tekst: ASCII
//    VTINT,        //! Wartośc stałoprzecinkowa   (int)                            tekst: wartośc numeryczna
//    VTFLOAT,      //! Wartość zmiennoprzecinkowa pojedynczej precyzji (float)     tekst: wartośc numeryczna
//    VTDOUBLE,     //! Wartość zmiennoprzecinkowa podwójnej precyzji (double)      tekst: wartośc numeryczna
//    VTSTRING,     //! Tekst (std::string)                                         tekst: ta sama wartość
//    VTDATE,       //! Data (boost::date_time::gregorian)                          tekst: DD-MM-RRRR
//    VTTIME,       //! Czas (boost::date_time::gregorian)                          tekst: HH:MM:SS:UUUU
//    VTDATETIME,   //! Data i czas (boost::date_time::gregorian)                   tekst: DD-MM-RRRR HH:MM:SS:UUUU
//    VTPHOTO       //! Zdjęcie (QIcon)                                             tekst: "Photo"
//};

////! Struktura opisująca pole danych
//struct FieldDescriptor
//{
//    //! Nazwa pola
//    std::string name;
//    //! Opis pola
//    std::string description;
//    //! Typ wartości pola
//    ValueType type;
//    //! Jednostka (jeśli wystepuje i dotyczy wartości, w przeciwnym razie nullptr)
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
////! Klasa przechowująca rózne wartości opisane w wyliczeniu ValueType
//class ValueAccessor
//{
//public:
//    //! KOnstruktor
//    //! \param value Wartość przechowywana
//    //! \param valueType typ wartości
//    //! \param valueAsString 
//    ValueAccessor(const boost::any & value, ValueType valueType, const std::string & valueAsString);
//
//    //! \return Wartość jako napis
//    const std::string & valueAsString() const;
//    //! \return Wartość
//    const boost::any & value() const;
//    //! \return Typ wartości
//    const ValueType type() const;
//
//private:
//    //! Wartość
//    boost::any value_;
//    //! Typ wartości
//    ValueType type_;
//    //! Wartość jako napis
//    std::string valueAsString_;
//};
//
////! Wartość danych zwracana z akcesorów - opisuje typ danych oraz ich wartośc w formie napisu
////typedef std::pair<ValueType, ValueAccessor> Value;
//typedef ValueAccessor Value;
//
////! Interfejs akcesora do pól danych wraz z ich opisem
//class IFieldAccessor
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IFieldAccessor() {}
//    //! Akcesor do danych z weryfikacją praw użytkownika do tych danych
//    //! \param fieldID Identyfikator pola do odczytania
//    //! \param shallowCopy Płytka kopia bazy danych
//    virtual const Value & field(int fieldID) const = 0;
//    //! \param idx Indeks opisu pola od 0 do fieldsCount() - 1
//    //! \return Opis pola danych
//    virtual const IndentifiableFieldDescriptor & fieldsDescriptor(int idx) const = 0;
//    //! \return Ilość dostępnych pól danych
//    virtual const int fieldsCount() const = 0;
//};
//
////! Akcesor pól identyfikowalny za pomocą unikalnego ID
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
//        VALUE,      //! Wartość atrybutu
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
//    //! \param idx Indeks atrybutu którego opisu szukamy z przedziału 0 - (attributesCount() - 1)
//    //! \return Opis atrybutu którego szukamy
//    virtual const IndentifiableFieldDescriptor & attributeDescriptor(int idx) const = 0;
//    //! \return Ilość dostępnych atrybutów
//    virtual const int attributesCount() const = 0;
//};
//
////! Interfejs akcesora atrybutów antropometrycznych
//class IAntropometricAttributes : public IAttributesAccessor
//{
//public:
//    //! Atrybuty antropometryczne
//    enum Attribute {
//        BodyMass,           //! Masa ciała
//        Height,             //! Wysokość
//        InterAsisDistance,  //! ?
//        LeftLegLength,      //! Długość lewej nogi
//        RightLegLenght,     //! Długośc prawej nogi
//        LeftKneeWidth,      //! Szerokość lewego kolana
//        RightKneeWidth,     //! Szerokość prawego kolana
//        LeftAnkleWidth,     //! Szerokość lewej kostki
//        RightAnkleWidth,    //! Szerokość prawej kostki
//        LeftCircuitThigh,   //! ?
//        RightCircuitThight, //! ?
//        LeftCircuitShank,   //! ?
//        RightCircuitShank,  //! ?
//        LeftShoulderOffset, //! ?
//        RightShoulderOffset,//! ?
//        LeftElbowWidth,     //! Szerokość lewego łokcia
//        RightElbowWidth,    //! Szerokość prawego łokcia
//        LeftWristWidth,     //! Szerokość lewego nadgarstka
//        RightWristWidth,    //! Szerokość prawego nadgarstka
//        LeftWristThickness, //! Wysokość lewego nadgarstka
//        RightWristThickness,//! Wysokość prawego nadgarstka
//        LeftHandWidth,      //! Szerokość lewej ręki
//        RightHandWidth,     //! Szerokośc prawej ręki
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
////! Interfejs akcesora do danych plików z uwzględnieniem praw użytkownika do danych
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
//    //! \return Właściciel pliku w hierarchi - jeden z podstawowych akcesorów obiektów które dziedzicza po IFiles
//    virtual const IFiles * owner() const = 0;
//};
//
////! Klasa bazowa elementów agregujących pliki
//class IFiles
//{
//public:
//    //! Destruktor wirtualny
//    virtual ~IFiles() {}
//    //! \return Ilość plików
//    virtual const int filesCount() const = 0;
//    //! \param idx Indeks pliku który chemy pobrać z przedziału 0 - (filesCount() - 1)
//    virtual const IFileAccessor * file(int idx) const = 0;
//};
//
////! Interfejs agregujący wystąpienia schorzeń
//class IDisorderOccurences
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IDisorderOccurences() {}
//    //! \return Ilosć wystąpień danego schorzenia
//    virtual const int disorderOccurencesCount() const = 0;
//    //! \param idx Indeks wystapienia schorzenia z przedziału 0 - (disorderOccurencesCount() - 1)
//    //! \return Wystąpienie schorzenia
//    virtual const IDisorderOccurenceAccessor * disorderOccurence(int idx) const = 0;
//};
//
////! Interfejs akcesora do danych schorzenia z uwzględnieniem praw użytkownika do danych
//class IDisorderAccessor : public IIdentifiableFieldAccessor, public IDisorderOccurences
//{
//public:
//    //! Pola informacji o schorzeniu
//    enum Field {
//        NAME       //! Nazwa schorzenia
//    };
//
//public:
//    //! \return Ilość pacjentów z tym schorzeniem
//    virtual const int patientsCount() const = 0;
//    //! \param idx Indeks pacjenta z przedziału 0 - (patientsCount() - 1)
//    //! \return Pacjenc o zadanym indeksie
//    virtual const IPatientAccessor * patient(int idx) const = 0;
//
//    //! Destruktor wirtualny
//    virtual ~IDisorderAccessor() {}
//};
//
////! Interfejs akcesora do danych wystąpienia schorzenia z uwzględnieniem praw użytkownika do danych
//class IDisorderOccurenceAccessor : public IIdentifiableFieldAccessor
//{
//public:
//    //! Pola informacji o wystąpieniu schorzeniu
//    enum Field {
//        FOCUS,          //! Istotność schorzenia w badaniu
//        DIAGNOSIS_DATE, //! Data diagnozy schorzenia
//        COMMENTS        //! Komentarz
//    };
//
//public:
//    //! \return Pacjent którego dotyczy to schorzenie
//    virtual const IPatientAccessor * patient() const = 0;
//    //! \return Schorzenie którego dotyczy ten opis wystąpienia schorzenia
//    virtual const IDisorderAccessor * disorder() const = 0;
//
//    //! Destruktor wirtualny
//    virtual ~IDisorderOccurenceAccessor() {}
//};
//
////! Interfejs akcesora do danych pacjenta z uwzględnieniem praw użytkownika do danych
//class IPatientAccessor : public IIdentifiableFieldAccessor, public IFiles, public IDisorderOccurences
//{
//public:
//    //! Pola informacji o pacjencie
//    enum Field {
//        NAME,       //! Nazwa pacjenta
//        SURNAME,    //! Nazwisko pacjenta
//        BIRTHDATE,  //! Data urodzenia pacjenta
//        GENDER,     //! Płeć pacjenta
//        PHOTO       //! Zdjęcie pacjenta
//    };
//
//public:
//    //! \return Subject i ruchy związane z danym pacjentem
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
//    //! \return Ilośc sesji subjecta
//    virtual const int sessionsCount() const = 0;
//    //! \param idx Indeks sesji z zakresu 0 - (sessionsCount() -1)
//    //! \return Akcesor sesji
//    virtual const ISessionAccessor * session(int idx) const = 0;
//};
//
////! Interfejs akcesora do danych subjecta z uwzględnieniem praw użytkownika do danych
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
////! Interfejs akcesora do danych laboratorium z uwzględnieniem praw użytkownika do danych
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
////! Interfejs akcesora do danych sesji z uwzględnieniem praw użytkownika do danych
//class ISessionAccessor : public IIdentifiableFieldAccessor, public IFiles, public IAntropometricAttributes
//{
//public:
//    //! Pola informacji o sesji
//    enum Field {
//        NAME,           //! Nazwa sesji
//        DATE,           //! Data przeprowadzenia sesji
//        OWNER,          //! Identyfikator właściciela sesji (zleceniodawcy)
//        MOTION_KIND,    //! Typ ruchu wykonywanego w sesji
//        TAGS,           //! Tagi sesji
//        DESCRIPTION     //! Opis sesji
//    };
//
//public:
//    //! \return Subject którego dotyczy ta sesja
//    virtual const ISubjectAccessor * subject() const = 0;
//    //! \return Laboratorium gdzie przeprowadzono sesję
//    virtual const ILabAccessor * lab() const = 0;
//    //! \return Ilość ruchów subjecta
//    virtual const int motionsCount() const = 0;
//    //! \param idx Indeks ruchu z przedziału 0 - (motionsCount() - 1)
//    //! \return Akcesor ruchu
//    virtual const IMotionAccessor * motion(int idx) const = 0;
//    //! Destruktor wirtualny
//    virtual ~ISessionAccessor() {}
//};
//
////! Interfejs akcesora do danych motiona z uwzględnieniem praw użytkownika do danych
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

////! Płytka kopia bazy danych ruchu uwzględniająca już prawa użytkownika do danych
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
////! Płytka kopia bazy danych medycznych uwzględniająca już prawa użytkownika do danych
//struct MedicalShallowCopy
//{
//public:
//    //! Schorzenia
//    Disorders disorders;
//    //! Wystąpienia schorzeń
//    DisordersOccurences occurences;
//    //! Pacjenci
//    Patients patients;
//    //! Destruktor
//    ~MedicalShallowCopy();
//};
//
////! Płytka kopia bazy danych uwzględniająca już prawa użytkownika do danych
//struct ShallowCopy {
//    MotionShallowCopy motionShallowCopy;
//    MedicalShallowCopy medicalShallowCopy;
//};

}

#endif	//	HEADER_GUARD___IDATASOURCESHALLOWCOPY_H__
