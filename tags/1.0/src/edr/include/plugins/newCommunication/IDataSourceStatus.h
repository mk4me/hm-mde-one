/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:24
    filename: IDataSourceStatus.h
    author:   Mateusz Janiak
    
    purpose:  Obiekty pozwalające określić status danych  - ich dostępnośc i użycie
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCESTATUS_H__
#define HEADER_GUARD___IDATASOURCESTATUS_H__

namespace communication
{

//! Status składowania danych
enum DataStorage {
    UnknownStorage = 0x00, /* Nieznany status - problem na serwerze, brak lokalnie */
    Local = 0x10,           /* Dane całkowicie lokalne */
    Remote = 0x01,          /* Dane całkoowice zdalne */
    PartiallyLocal = 0x11   /* Dane częsciowo dostępne lokalnie*/
};

//! Status użycia danych w DM
enum DataUsage {
    UnknownUsage = 0x00,    /* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie może się pojawić dla danych */
    Loaded = 0x10,          /* Dane załadowane w całości do DM */
    Unloaded = 0x01,        /* Dane nie załadowane do DM */
    PartiallyLoaded = 0x11  /* Dane częsciowo załadowane do DM */
};

//! Interfejs dostarczający informacji o statusie poszczególnych elementów hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan składowania danych: Lokalne, Zdalne, Częsciowo lokalne, Nieznany (problem z bazą, wymaganiami dla danych)
//! Przechowuje stan użycia danych: Załadowane, Niezaładowane, Częsciowo załadowane
class IDataStatus
{
public:

    //! Wirtualny destruktor
    virtual ~IDataStatus() {};

    //! \return Stan składowania danych
    virtual const DataStorage storage() const = 0;

    //! \return Stan użycia danych w DM
    virtual const DataUsage usage() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCESTATUS_H__
