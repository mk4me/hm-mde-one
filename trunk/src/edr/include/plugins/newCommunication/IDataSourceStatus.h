/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:24
    filename: IDataSourceStatus.h
    author:   Mateusz Janiak
    
    purpose:  Obiekty pozwalaj¹ce okreœliæ status danych  - ich dostepnoœc i u¿ycie
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCESTATUS_H__
#define HEADER_GUARD___IDATASOURCESTATUS_H__

namespace communication
{

//! Status sk³adowania danych
enum DataStorage {
    UnknownStorage = 0x00, /* Nieznany status - problem na serwerze, brak lokalnie */
    Local = 0x10,           /* Dane ca³kowicie lokalne */
    Remote = 0x01,          /* Dane ca³koowice zdalne */
    PartiallyLocal = 0x11   /* Dane czêsciowo dostepne lokalnie*/
};

//! Status u¿ycia danych w DM
enum DataUsage {
    UnknownUsage = 0x00,    /* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie mo¿e siê pojawiæ dla danych */
    Loaded = 0x10,          /* Dane za³adowane w ca³oœci do DM */
    Unloaded = 0x01,        /* Dane nie za³adowane do DM */
    PartiallyLoaded = 0x11  /* Dane czêsciowo za³adowane do DM */
};

//! Interfejs dostarczaj¹cy informacji o statusie poszczególnych elementów hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan sk³adowania danych: Lokalne, Zdalne, Czêsciowo lokalne, Nieznany (problem z baz¹, wymaganiami dla danych)
//! Przechowuje stan u¿ycia danych: Za³adowane, Nieza³adowane, Czêsciowo za³adowane
class IDataStatus
{
public:

    //! Wirtualny destruktor
    virtual ~IDataStatus() {};

    //! \return Stan sk³adowania danych
    virtual const DataStorage storage() const = 0;

    //! \return Stan u¿ycia danych w DM
    virtual const DataUsage usage() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCESTATUS_H__
