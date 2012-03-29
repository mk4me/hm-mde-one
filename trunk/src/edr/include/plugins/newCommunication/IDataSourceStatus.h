/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:24
    filename: IDataSourceStatus.h
    author:   Mateusz Janiak
    
    purpose:  Obiekty pozwalaj�ce okre�li� status danych  - ich dostepno�c i u�ycie
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCESTATUS_H__
#define HEADER_GUARD___IDATASOURCESTATUS_H__

namespace communication
{

//! Status sk�adowania danych
enum DataStorage {
    UnknownStorage = 0x00, /* Nieznany status - problem na serwerze, brak lokalnie */
    Local = 0x10,           /* Dane ca�kowicie lokalne */
    Remote = 0x01,          /* Dane ca�koowice zdalne */
    PartiallyLocal = 0x11   /* Dane cz�sciowo dostepne lokalnie*/
};

//! Status u�ycia danych w DM
enum DataUsage {
    UnknownUsage = 0x00,    /* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie mo�e si� pojawi� dla danych */
    Loaded = 0x10,          /* Dane za�adowane w ca�o�ci do DM */
    Unloaded = 0x01,        /* Dane nie za�adowane do DM */
    PartiallyLoaded = 0x11  /* Dane cz�sciowo za�adowane do DM */
};

//! Interfejs dostarczaj�cy informacji o statusie poszczeg�lnych element�w hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan sk�adowania danych: Lokalne, Zdalne, Cz�sciowo lokalne, Nieznany (problem z baz�, wymaganiami dla danych)
//! Przechowuje stan u�ycia danych: Za�adowane, Nieza�adowane, Cz�sciowo za�adowane
class IDataStatus
{
public:

    //! Wirtualny destruktor
    virtual ~IDataStatus() {};

    //! \return Stan sk�adowania danych
    virtual const DataStorage storage() const = 0;

    //! \return Stan u�ycia danych w DM
    virtual const DataUsage usage() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCESTATUS_H__
