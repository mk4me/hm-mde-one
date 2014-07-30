/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:24
    filename: DataStatus.h
    author:   Mateusz Janiak
    
    purpose:  Obiekty pozwalające określić status danych  - ich dostępnośc i użycie
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DATASTATUS_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DATASTATUS_H__

namespace hmdbCommunication
{

//! Status składowania danych
enum DataStorage {
    UnknownStorage = 0x00, /* Nieznany status - problem na serwerze, brak lokalnie */
    Local = 0x01,           /* Dane całkowicie lokalne */
    Remote = 0x10,          /* Dane całkoowice zdalne */
    PartiallyLocal = 0x11   /* Dane częsciowo dostępne lokalnie*/
};

//! Status użycia danych w DM
enum DataUsage {
    UnknownUsage = 0x00,    /* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie może się pojawić dla danych */
    Loaded = 0x01,          /* Dane załadowane w całości do DM */
    Unloaded = 0x10,        /* Dane nie załadowane do DM */
    PartiallyLoaded = 0x11  /* Dane częsciowo załadowane do DM */
};

//! Status ważności danych
enum DataValidity {
	UnknownValidity = 0x00,	/* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie może się pojawić dla danych */
	Valid = 0x01,			/* Dane ważne - zsynchronizowane z bazą */
	Outdated = 0x10,        /* Dane nieważne - należy ściągnąć i załadowac ponownie */
	PartiallyValid = 0x11	/* Dane częsciowo zsynchronizowane z bazą */
};

//! Klasa dostarczający informacji o statusie poszczególnych elementów hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan składowania danych: Lokalne, Zdalne, Częsciowo lokalne, Nieznany (problem z bazą, wymaganiami dla danych)
//! Przechowuje stan użycia danych: Załadowane, Niezaładowane, Częsciowo załadowane
class DataStatus
{
public:
	//! Status częsciowy dla wszystkich parametrów
	static const DataStatus PartialStatus;

public:

	//! Konstruktor kopiujący
	DataStatus(const DataStatus & status);

	//! \param storage Stan przechowywania danych
	//! \param usage Stan załadowania danych
	//! \param validity Stan ważności danych
	DataStatus(const DataStorage storage = UnknownStorage,
		const DataUsage usage = UnknownUsage,
		const DataValidity validity = UnknownValidity);

	//! \param storage Stan przechowywania danych
	explicit DataStatus(const DataStorage storage);
	//! \param usage Stan załadowania danych
	explicit DataStatus(const DataUsage usage);
	//! \param validity Stan ważności danych
	explicit DataStatus(const DataValidity validity);

    //! Wirtualny destruktor
	~DataStatus();

    //! \return Stan składowania danych
    const DataStorage storage() const;

    //! \return Stan użycia danych w DM
    const DataUsage usage() const;

	//! \return Stan ważności danych
	const DataValidity validity() const;

	//! Łączy logicznie statusy
	const DataStatus & merge(const DataStatus & status);
	//! Łączy logicznie statusy
	const DataStorage merge(const DataStorage storage);
	//! Łączy logicznie statusy
	const DataUsage merge(const DataUsage usage);
	//! Łączy logicznie statusy
	const DataValidity merge(const DataValidity validity);

	//! Operatory konwersji
	/*
	operator DataStorage();
	operator DataUsage();
	operator DataValidity();
	*/

	//! Operator mniejszości
	bool operator<(const hmdbCommunication::DataStatus & b) const;

private:
	//! Status składowania danych
	DataStorage storage_;
	//! Status użycia danych w DM
	DataUsage usage_;
	//! Ważnośc danych
	DataValidity validity_;
};

}

//! Operator równości statusów
bool operator==(const hmdbCommunication::DataStatus & a, const hmdbCommunication::DataStatus & b);
//! Operator różności statusów
bool operator!=(const hmdbCommunication::DataStatus & a, const hmdbCommunication::DataStatus & b);
//! Operator mniejszości
bool operator<(const hmdbCommunication::DataStatus & a, const hmdbCommunication::DataStatus & b);

hmdbCommunication::DataStorage operator|(hmdbCommunication::DataStorage l, hmdbCommunication::DataStorage r);
hmdbCommunication::DataStorage & operator|=(hmdbCommunication::DataStorage & l, hmdbCommunication::DataStorage r);
hmdbCommunication::DataUsage operator|(hmdbCommunication::DataUsage l, hmdbCommunication::DataUsage r);
hmdbCommunication::DataUsage & operator|=(hmdbCommunication::DataUsage & l, hmdbCommunication::DataUsage r);
hmdbCommunication::DataValidity operator|(hmdbCommunication::DataValidity l, hmdbCommunication::DataValidity r);
hmdbCommunication::DataValidity & operator|=(hmdbCommunication::DataValidity & l, hmdbCommunication::DataValidity r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus & r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStorage & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStorage & r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataUsage & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataUsage & r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataValidity & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataValidity & r);

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DATASTATUS_H__
