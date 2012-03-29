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

//! Klasa dostarczaj�ca informacji o statusie poszczeg�lnych element�w hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan sk�adowania danych: Lokalne, Zdalne, Cz�sciowo lokalne
//! Przechowuje stan u�ycia danych: Za�adowane, Nieza�adowane, Cz�sciowo za�adowane
class DataStatus : public communication::IDataStatus
{
    friend bool operator==(const DataStatus & a, const DataStatus & b);
    friend bool operator!=(const DataStatus & a, const DataStatus & b);

public:
    //! Konstruktor kopiuj�cy
    DataStatus(const DataStatus & status);
    //! Domy�lny konstruktor
    //! \param storage Stan sk��dowania danych
    //! \param usage Stan u�ycia danych
    DataStatus(communication::DataStorage storage = communication::UnknownStorage, communication::DataUsage usage = communication::UnknownUsage);

    //! Wirtualny destruktor
    virtual ~DataStatus();

    //! \param storage Nowy stan sk�adowania danych
    void setStorage(communication::DataStorage storage);
    //! \return stan sk��dowania danych
    virtual const communication::DataStorage storage() const;

    //! \param usage Nowy stan u�ycia danych w DM
    void setUsage(communication::DataUsage usage);
    //! \return stan u�ycia danych w DM
    virtual const communication::DataUsage usage() const;

    //! \param status Warto�� do przypisania
    //! \return obiekt po przypisaniu danych
    DataStatus & operator=(const DataStatus & status);

private:
    //! Status sk�adowania danych
    communication::DataStorage storage_;
    //! Status u�ycia danych w DM
    communication::DataUsage usage_;
};

//! Operator r�wno�ci status�w
bool operator==(const DataStatus & a, const DataStatus & b);
//! Operator r�no�ci status�w
bool operator!=(const DataStatus & a, const DataStatus & b);

communication::DataStorage operator|(communication::DataStorage l, communication::DataStorage r);
communication::DataStorage & operator|=(communication::DataStorage & l, communication::DataStorage r);
communication::DataUsage operator|(communication::DataUsage l, communication::DataUsage r);
communication::DataUsage & operator|=(communication::DataUsage & l, communication::DataUsage r);
DataStatus operator|(const DataStatus & l, const DataStatus & r);
DataStatus & operator|=(DataStatus & l, const DataStatus & r);

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCESTATUS_H__