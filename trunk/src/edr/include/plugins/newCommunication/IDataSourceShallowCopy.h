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

//! Typ wartosci pochodz�cych z bazy danych
//enum ValueType {
//    VTBOOL,       //! Warto�� prawda/fa�sz (bool)                                 tekst: "true", "false"
//    VTCHAR,       //! Znak (char)                                                 tekst: ASCII
//    VTINT,        //! Warto�c sta�oprzecinkowa   (int)                            tekst: warto�c numeryczna
//    VTFLOAT,      //! Warto�� zmiennoprzecinkowa pojedynczej precyzji (float)     tekst: warto�c numeryczna
//    VTDOUBLE,     //! Warto�� zmiennoprzecinkowa podw�jnej precyzji (double)      tekst: warto�c numeryczna
//    VTSTRING,     //! Tekst (std::string)                                         tekst: ta sama warto��
//    VTDATE,       //! Data (boost::date_time::gregorian)                          tekst: DD-MM-RRRR
//    VTTIME,       //! Czas (boost::date_time::gregorian)                          tekst: HH:MM:SS:UUUU
//    VTDATETIME,   //! Data i czas (boost::date_time::gregorian)                   tekst: DD-MM-RRRR HH:MM:SS:UUUU
//    VTPHOTO       //! Zdj�cie (QIcon)                                             tekst: "Photo"
//};

////! Struktura opisuj�ca pole danych
//struct FieldDescriptor
//{
//    //! Nazwa pola
//    std::string name;
//    //! Opis pola
//    std::string description;
//    //! Typ warto�ci pola
//    ValueType type;
//    //! Jednostka (je�li wystepuje i dotyczy warto�ci, w przeciwnym razie nullptr)
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
////! Klasa przechowuj�ca r�zne warto�ci opisane w wyliczeniu ValueType
//class ValueAccessor
//{
//public:
//    //! KOnstruktor
//    //! \param value Warto�� przechowywana
//    //! \param valueType typ warto�ci
//    //! \param valueAsString 
//    ValueAccessor(const boost::any & value, ValueType valueType, const std::string & valueAsString);
//
//    //! \return Warto�� jako napis
//    const std::string & valueAsString() const;
//    //! \return Warto��
//    const boost::any & value() const;
//    //! \return Typ warto�ci
//    const ValueType type() const;
//
//private:
//    //! Warto��
//    boost::any value_;
//    //! Typ warto�ci
//    ValueType type_;
//    //! Warto�� jako napis
//    std::string valueAsString_;
//};
//
////! Warto�� danych zwracana z akcesor�w - opisuje typ danych oraz ich warto�c w formie napisu
////typedef std::pair<ValueType, ValueAccessor> Value;
//typedef ValueAccessor Value;
//
////! Interfejs akcesora do p�l danych wraz z ich opisem
//class IFieldAccessor
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IFieldAccessor() {}
//    //! Akcesor do danych z weryfikacj� praw u�ytkownika do tych danych
//    //! \param fieldID Identyfikator pola do odczytania
//    //! \param shallowCopy P�ytka kopia bazy danych
//    virtual const Value & field(int fieldID) const = 0;
//    //! \param idx Indeks opisu pola od 0 do fieldsCount() - 1
//    //! \return Opis pola danych
//    virtual const IndentifiableFieldDescriptor & fieldsDescriptor(int idx) const = 0;
//    //! \return Ilo�� dost�pnych p�l danych
//    virtual const int fieldsCount() const = 0;
//};
//
////! Akcesor p�l identyfikowalny za pomoc� unikalnego ID
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
////! Interfejs akcesora do atrybut�w antropometrycznych
//class IAntropometricAccessor : public IIdentifiableFieldAccessor
//{
//public:
//    //! Pola danych atrybutu antropometrycznego
//    enum Field {
//        NAME,       //! Nazwa atrybutu
//        VALUE,      //! Warto�� atrybutu
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
//    //! \param idx Indeks atrybutu kt�rego opisu szukamy z przedzia�u 0 - (attributesCount() - 1)
//    //! \return Opis atrybutu kt�rego szukamy
//    virtual const IndentifiableFieldDescriptor & attributeDescriptor(int idx) const = 0;
//    //! \return Ilo�� dost�pnych atrybut�w
//    virtual const int attributesCount() const = 0;
//};
//
////! Interfejs akcesora atrybut�w antropometrycznych
//class IAntropometricAttributes : public IAttributesAccessor
//{
//public:
//    //! Atrybuty antropometryczne
//    enum Attribute {
//        BodyMass,           //! Masa cia�a
//        Height,             //! Wysoko��
//        InterAsisDistance,  //! ?
//        LeftLegLength,      //! D�ugo�� lewej nogi
//        RightLegLenght,     //! D�ugo�c prawej nogi
//        LeftKneeWidth,      //! Szeroko�� lewego kolana
//        RightKneeWidth,     //! Szeroko�� prawego kolana
//        LeftAnkleWidth,     //! Szeroko�� lewej kostki
//        RightAnkleWidth,    //! Szeroko�� prawej kostki
//        LeftCircuitThigh,   //! ?
//        RightCircuitThight, //! ?
//        LeftCircuitShank,   //! ?
//        RightCircuitShank,  //! ?
//        LeftShoulderOffset, //! ?
//        RightShoulderOffset,//! ?
//        LeftElbowWidth,     //! Szeroko�� lewego �okcia
//        RightElbowWidth,    //! Szeroko�� prawego �okcia
//        LeftWristWidth,     //! Szeroko�� lewego nadgarstka
//        RightWristWidth,    //! Szeroko�� prawego nadgarstka
//        LeftWristThickness, //! Wysoko�� lewego nadgarstka
//        RightWristThickness,//! Wysoko�� prawego nadgarstka
//        LeftHandWidth,      //! Szeroko�� lewej r�ki
//        RightHandWidth,     //! Szeroko�c prawej r�ki
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
////! Interfejs akcesora do danych plik�w z uwzgl�dnieniem praw u�ytkownika do danych
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
//    //! \return W�a�ciciel pliku w hierarchi - jeden z podstawowych akcesor�w obiekt�w kt�re dziedzicza po IFiles
//    virtual const IFiles * owner() const = 0;
//};
//
////! Klasa bazowa element�w agreguj�cych pliki
//class IFiles
//{
//public:
//    //! Destruktor wirtualny
//    virtual ~IFiles() {}
//    //! \return Ilo�� plik�w
//    virtual const int filesCount() const = 0;
//    //! \param idx Indeks pliku kt�ry chemy pobra� z przedzia�u 0 - (filesCount() - 1)
//    virtual const IFileAccessor * file(int idx) const = 0;
//};
//
////! Interfejs agreguj�cy wyst�pienia schorze�
//class IDisorderOccurences
//{
//public:
//    //! Wirtualny destruktor
//    virtual ~IDisorderOccurences() {}
//    //! \return Ilos� wyst�pie� danego schorzenia
//    virtual const int disorderOccurencesCount() const = 0;
//    //! \param idx Indeks wystapienia schorzenia z przedzia�u 0 - (disorderOccurencesCount() - 1)
//    //! \return Wyst�pienie schorzenia
//    virtual const IDisorderOccurenceAccessor * disorderOccurence(int idx) const = 0;
//};
//
////! Interfejs akcesora do danych schorzenia z uwzgl�dnieniem praw u�ytkownika do danych
//class IDisorderAccessor : public IIdentifiableFieldAccessor, public IDisorderOccurences
//{
//public:
//    //! Pola informacji o schorzeniu
//    enum Field {
//        NAME       //! Nazwa schorzenia
//    };
//
//public:
//    //! \return Ilo�� pacjent�w z tym schorzeniem
//    virtual const int patientsCount() const = 0;
//    //! \param idx Indeks pacjenta z przedzia�u 0 - (patientsCount() - 1)
//    //! \return Pacjenc o zadanym indeksie
//    virtual const IPatientAccessor * patient(int idx) const = 0;
//
//    //! Destruktor wirtualny
//    virtual ~IDisorderAccessor() {}
//};
//
////! Interfejs akcesora do danych wyst�pienia schorzenia z uwzgl�dnieniem praw u�ytkownika do danych
//class IDisorderOccurenceAccessor : public IIdentifiableFieldAccessor
//{
//public:
//    //! Pola informacji o wyst�pieniu schorzeniu
//    enum Field {
//        FOCUS,          //! Istotno�� schorzenia w badaniu
//        DIAGNOSIS_DATE, //! Data diagnozy schorzenia
//        COMMENTS        //! Komentarz
//    };
//
//public:
//    //! \return Pacjent kt�rego dotyczy to schorzenie
//    virtual const IPatientAccessor * patient() const = 0;
//    //! \return Schorzenie kt�rego dotyczy ten opis wyst�pienia schorzenia
//    virtual const IDisorderAccessor * disorder() const = 0;
//
//    //! Destruktor wirtualny
//    virtual ~IDisorderOccurenceAccessor() {}
//};
//
////! Interfejs akcesora do danych pacjenta z uwzgl�dnieniem praw u�ytkownika do danych
//class IPatientAccessor : public IIdentifiableFieldAccessor, public IFiles, public IDisorderOccurences
//{
//public:
//    //! Pola informacji o pacjencie
//    enum Field {
//        NAME,       //! Nazwa pacjenta
//        SURNAME,    //! Nazwisko pacjenta
//        BIRTHDATE,  //! Data urodzenia pacjenta
//        GENDER,     //! P�e� pacjenta
//        PHOTO       //! Zdj�cie pacjenta
//    };
//
//public:
//    //! \return Subject i ruchy zwi�zane z danym pacjentem
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
//    //! \return Ilo�c sesji subjecta
//    virtual const int sessionsCount() const = 0;
//    //! \param idx Indeks sesji z zakresu 0 - (sessionsCount() -1)
//    //! \return Akcesor sesji
//    virtual const ISessionAccessor * session(int idx) const = 0;
//};
//
////! Interfejs akcesora do danych subjecta z uwzgl�dnieniem praw u�ytkownika do danych
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
////! Forward declaration interfejsu akcesora do motion�w
//class IMotionAccessor;
//
////! Interfejs akcesora do danych laboratorium z uwzgl�dnieniem praw u�ytkownika do danych
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
////! Interfejs akcesora do danych sesji z uwzgl�dnieniem praw u�ytkownika do danych
//class ISessionAccessor : public IIdentifiableFieldAccessor, public IFiles, public IAntropometricAttributes
//{
//public:
//    //! Pola informacji o sesji
//    enum Field {
//        NAME,           //! Nazwa sesji
//        DATE,           //! Data przeprowadzenia sesji
//        OWNER,          //! Identyfikator w�a�ciciela sesji (zleceniodawcy)
//        MOTION_KIND,    //! Typ ruchu wykonywanego w sesji
//        TAGS,           //! Tagi sesji
//        DESCRIPTION     //! Opis sesji
//    };
//
//public:
//    //! \return Subject kt�rego dotyczy ta sesja
//    virtual const ISubjectAccessor * subject() const = 0;
//    //! \return Laboratorium gdzie przeprowadzono sesj�
//    virtual const ILabAccessor * lab() const = 0;
//    //! \return Ilo�� ruch�w subjecta
//    virtual const int motionsCount() const = 0;
//    //! \param idx Indeks ruchu z przedzia�u 0 - (motionsCount() - 1)
//    //! \return Akcesor ruchu
//    virtual const IMotionAccessor * motion(int idx) const = 0;
//    //! Destruktor wirtualny
//    virtual ~ISessionAccessor() {}
//};
//
////! Interfejs akcesora do danych motiona z uwzgl�dnieniem praw u�ytkownika do danych
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
//    //! \return Sesja kt�rej dotyczy ten motion
//    virtual const ISessionAccessor * session() const = 0;
//    //! Destruktor wirtualny
//    virtual ~IMotionAccessors() {}
//};
//
////! Mapy obiekt�w danych ruchu - ID <-> obiekt
//typedef std::map<int, IFileAccessor *> Files;
//typedef std::map<int, IMotionAccessor *> Motions;
//typedef std::map<int, ISessionAccessor *> Sessions;
//typedef std::map<int, ISubjectAccessor *> Subjects;
//typedef std::map<int, ILabAccessor *> Labs;

////! P�ytka kopia bazy danych ruchu uwzgl�dniaj�ca ju� prawa u�ytkownika do danych
//struct MotionShallowCopy
//{
//public:
//    //! Laboratoria
//    Labs labs;
//    //! Subjecty
//    Subjects subjects;
//    //! Sesje subject�w
//    Sessions sessions;
//    //! Nagrania ruch�w
//    Motions motions;
//    //! Pliki
//    Files files;
//
//    //! Destruktor
//    ~MotionShallowCopy();
//};
//
////! Mapy obiekt�w danych medycznych - ID <-> obiekt
//typedef std::map<int, IDisorderAccessor *> Disorders;
//typedef std::map<int, IDisorderOccurenceAccessor *> DisordersOccurences;
//typedef std::map<int, IPatientAccessor *> Patients;
//
////! P�ytka kopia bazy danych medycznych uwzgl�dniaj�ca ju� prawa u�ytkownika do danych
//struct MedicalShallowCopy
//{
//public:
//    //! Schorzenia
//    Disorders disorders;
//    //! Wyst�pienia schorze�
//    DisordersOccurences occurences;
//    //! Pacjenci
//    Patients patients;
//    //! Destruktor
//    ~MedicalShallowCopy();
//};
//
////! P�ytka kopia bazy danych uwzgl�dniaj�ca ju� prawa u�ytkownika do danych
//struct ShallowCopy {
//    MotionShallowCopy motionShallowCopy;
//    MedicalShallowCopy medicalShallowCopy;
//};

}

#endif	//	HEADER_GUARD___IDATASOURCESHALLOWCOPY_H__
