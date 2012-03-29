/********************************************************************
    created:  2012/02/03
    created:  3:2:2012   17:29
    filename: CommunicationDataSourceStatus.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCESTATUS_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCESTATUS_H__

#include <plugins/newCommunication/IDataSourceStatus.h>

//! Klasa dostarczaj¹ca informacji o statusie poszczególnych elementów hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan sk³adowania danych: Lokalne, Zdalne, Czêsciowo lokalne
//! Przechowuje stan u¿ycia danych: Za³adowane, Nieza³adowane, Czêsciowo za³adowane
class DataStatus : public communication::IDataStatus
{
    friend bool operator==(const DataStatus & a, const DataStatus & b);
    friend bool operator!=(const DataStatus & a, const DataStatus & b);

public:
    //! Konstruktor kopiuj¹cy
    DataStatus(const DataStatus & status);
    //! Domyœlny konstruktor
    //! \param storage Stan sk³¹dowania danych
    //! \param usage Stan u¿ycia danych
    DataStatus(communication::DataStorage storage = communication::UnknownStorage, communication::DataUsage usage = communication::UnknownUsage);

    //! Wirtualny destruktor
    virtual ~DataStatus();

    //! \param storage Nowy stan sk³adowania danych
    void setStorage(communication::DataStorage storage);
    //! \return stan sk³¹dowania danych
    virtual const communication::DataStorage storage() const;

    //! \param usage Nowy stan u¿ycia danych w DM
    void setUsage(communication::DataUsage usage);
    //! \return stan u¿ycia danych w DM
    virtual const communication::DataUsage usage() const;

    //! \param status Wartoœæ do przypisania
    //! \return obiekt po przypisaniu danych
    DataStatus & operator=(const DataStatus & status);

private:
    //! Status sk³adowania danych
    communication::DataStorage storage_;
    //! Status u¿ycia danych w DM
    communication::DataUsage usage_;
};

//! Operator równoœci statusów
bool operator==(const DataStatus & a, const DataStatus & b);
//! Operator ró¿noœci statusów
bool operator!=(const DataStatus & a, const DataStatus & b);

communication::DataStorage operator|(communication::DataStorage l, communication::DataStorage r);
communication::DataStorage & operator|=(communication::DataStorage & l, communication::DataStorage r);
communication::DataUsage operator|(communication::DataUsage l, communication::DataUsage r);
communication::DataUsage & operator|=(communication::DataUsage & l, communication::DataUsage r);
DataStatus operator|(const DataStatus & l, const DataStatus & r);
DataStatus & operator|=(DataStatus & l, const DataStatus & r);

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCESTATUS_H__