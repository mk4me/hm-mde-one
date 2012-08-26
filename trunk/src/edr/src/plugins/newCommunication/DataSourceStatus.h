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

//! Klasa dostarczająca informacji o statusie poszczególnych elementów hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan składowania danych: Lokalne, Zdalne, Częsciowo lokalne
//! Przechowuje stan użycia danych: Załadowane, Niezaładowane, Częsciowo załadowane
class DataStatus : public communication::IDataStatus
{
    friend bool operator==(const DataStatus & a, const DataStatus & b);
    friend bool operator!=(const DataStatus & a, const DataStatus & b);

public:
    //! Konstruktor kopiujący
    DataStatus(const DataStatus & status);
    //! Domyślny konstruktor
    //! \param storage Stan skłądowania danych
    //! \param usage Stan użycia danych
    DataStatus(communication::DataStorage storage = communication::UnknownStorage, communication::DataUsage usage = communication::UnknownUsage);

    //! Wirtualny destruktor
    virtual ~DataStatus();

    //! \param storage Nowy stan składowania danych
    void setStorage(communication::DataStorage storage);
    //! \return stan skłądowania danych
    virtual const communication::DataStorage storage() const;

    //! \param usage Nowy stan użycia danych w DM
    void setUsage(communication::DataUsage usage);
    //! \return stan użycia danych w DM
    virtual const communication::DataUsage usage() const;

    //! \param status Wartość do przypisania
    //! \return obiekt po przypisaniu danych
    DataStatus & operator=(const DataStatus & status);

private:
    //! Status składowania danych
    communication::DataStorage storage_;
    //! Status użycia danych w DM
    communication::DataUsage usage_;
};

//! Operator równości statusów
bool operator==(const DataStatus & a, const DataStatus & b);
//! Operator różności statusów
bool operator!=(const DataStatus & a, const DataStatus & b);

communication::DataStorage operator|(communication::DataStorage l, communication::DataStorage r);
communication::DataStorage & operator|=(communication::DataStorage & l, communication::DataStorage r);
communication::DataUsage operator|(communication::DataUsage l, communication::DataUsage r);
communication::DataUsage & operator|=(communication::DataUsage & l, communication::DataUsage r);
DataStatus operator|(const DataStatus & l, const DataStatus & r);
DataStatus & operator|=(DataStatus & l, const DataStatus & r);

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCESTATUS_H__
