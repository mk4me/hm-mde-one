#ifndef HEADER_GUARD__PIN_H__
#define HEADER_GUARD__PIN_H__

#include <dfmlib/DFLMTypes.h>
#include <set>
#include <vector>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm {
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentuj�ca punkt ��czenia w�z��w. Rozr�niamy 2 typy pin�w - wej�ciowe i wyj�ciowe.
class Pin : public boost::enable_shared_from_this<Pin>
{
public:

    //! Typ okre�laj�cy typ pinu
	enum PinType {
        IN, //! Pin wejsciowy
        OUT, //! Pin wyj�ciowy
        UNSET //! Pin niezdefiniowany
    };

    //! Typ okre�laj�cy statyczny stan pinu
	enum StaticStatus {
        OK,         //! Z pinem wszystko OK, neutralny stan pinu niepod��czonego bez �adnych wymaga� (zale�no�ci, wymaganie pod�aczenia)
        REQUIRED,   //! Pin jest wymagany a nie jest pod��czony (dotyczy pin�w wej�ciowych)
        INCOMPLETE, //! Pin ma niespe�nione zale�no�ci - jeden z pin�w od kt�rych jest zale�ny nie jest pod��czony (dotyczy pin�w wyj�ciowych)
        CONNECTED   //! Pin poprawnie pod��czony
    };

    //! Typ okre�laj�cy dynamiczny stan pinu podczas ��czenia
	enum DynamicStatus {
        COMPATIBLE,         //! Pin kompatybilny - mo�na ��czy�
        COMPATIBLE_CYCLE,   //! Pin kompatybilny ale tworzy cykl - nie mo�na ��czy�
        COMPATIBLE_FULL,    //! Pin kompatybilny ale docelowy pin wej�ciowy ju� zaj�ty - nie mo�na ��czy�
        INCOMPATIBLE        //! Pin nmiekompatybilny - nie mo�na ��czy�
    };

    //! Typ kolekcji po��cze�
	//typedef std::set<ConnPtr> Connections;
    typedef std::vector<ConnPtr> Connections;

    //! Typ kolekcji pion�w wymaganych
	typedef std::set<WPinPtr> ReqPinsSet;

    //! Typ iteratora do kolekcji po��cze�
    typedef Connections::const_iterator iterator;

    //! Typ ilo�ci po��cze� w kolekcji
    typedef Connections::size_type size_type;

private:

    //! Funktor zwracaj�cy indeks pinu
    typedef boost::function<int(const PinPtr &)> PinIndexFunc;

public:

    //! Zaprzyja�niony typ w�z�a - one zarz�dzaj� swoimi pinami i na ich bazie s� �aczone
	friend class Node;

    //! Zaprzyja�niony typ modelu - zarz�dza, weryfikuje stan modelu, regu�y jego ��czenia mi�dzy innymi przez piny
	friend class Model;

public:

    //! Konstruktor domy�lny
    //! \param name nazwa pinu
    //! \param required Czy pin wymagany (dotyczy tylko pin�w wejsciowych)
    //! \param requiredPins Piny od kt�rych jest zale�ny aktualny pin (dotyczy pin�w wyj�ciowych)
	Pin(const std::string & name = std::string(),
		bool required = false, const ReqPinsSet & requiredPins = ReqPinsSet());
	
    //! Wirtualny destruktor
	virtual ~Pin(void);

    //! \return Wska�nik do rodzica lub nullptr je�li pin nie jest dodany do w�z�a
	NPtr getParent() const;

    //! \return Iterator pocz�tka kolekcji po��cze�
    iterator begin() const;

    //! \return Iterator ko�ca kolekcji po��cze�
    iterator end() const;

    //! \param idx Indeks po��czenia
    //! \return Zwraca po�aczenie o zadanym indeksie
    const ConnPtr & getConnection(int idx) const;

    //! \param connection Po�aczenie kt�rego indeksu szukamy
    //! \return Indeks po��czenia w kolekcji lub -1 je�li po��czenia nie ma w kolekcji
    int getIndexOfConnection(const ConnPtr & connection) const;

    //! \return Czy kolekcja po��cze� jest pusta
    bool empty() const;

    //! \return Ilo�� po��cze� w kolekcji
    size_type size() const;

    //! \return Typ pinu (IN|OUT)
	PinType getType() const;

    //! \return Statyczny status pinu
	StaticStatus getStaticStatus() const;

    //! \param refPin Pin wzgledem ktorego badamy stan dynamiczny pinu (podczas ��czenia)
    //! \param model Model w obr�bie kt�rego si� poruszamy definiuj�c status dynamiczny
    //! \return Status dynamiczny pinu przed prob� po��czenia
	DynamicStatus getDynamicStatus(const PinPtr & refPin, const MPtr & model) const;

    //! \param pin Pin kt�rego kompatybilno�c testujemy
    //! \return Prawda je�li piny s� kompatybilne (aktualny pin potrafi obs�u�y� testowany pin)
	virtual bool isCompatible(const CPinPtr & pin) const;
	
    //! \param pin Pin kt�rego testujemy czy aktualny pin jest od niego zale�ny (dotyczy pin�w wyjsciowych)
    //! \returen Prawda je�li aktualny pin jest zale�ny od zadanego pinu
	bool dependsOnPin(const PinPtr & pin) const;

    //! \return Zwraca zbi�r pin�w od kt�rych jest uzale�niony aktualny pin
	const ReqPinsSet & getDependantPins() const;

    //! \return Czy dany pin jest wymagany (dotyczy pin�w wej�ciowych)
	bool isRequired() const;

    //! \return Czy wszystkie zale�no�ci pinu s� spe�nione (dotyczy pin�w wyj�ciowych)
	bool isComplete() const;

    //! \return Nazwa pinu
	const std::string & getName() const;

 //   //! \param Nazwa pinu
	//void setPinName(const std::string & name);

    //! \return Indeks pinu w w�le (-1 je�li pin nigdzie nie dodany, automatycznie ropoznaje typ pinu
    int getPinIndex() const;

private:

    //! \param name Nowa nazwa pinu
    void setName(const std::string & name);

    //! \param connection Po��czenie dodawane do pinu
    void addConnection(const ConnPtr & connection);

    //! \param connection Po��czenie usuwane z pinu
    void removeConnection(const ConnPtr & connection);

    //! Usuwa wszystkie po��czenia pinu
	void clearConnections();

    //! \param type Typ pinu ustawiany podczas do�anczania go do w�z�a
	void setType(PinType type);

private:

    //! Zbi�r po��cze� pinu
	Connections connections;

    //! W�ze� rodzic pinu
	WNPtr parentNode;

    //! Czy pin wymagany (dla pin �w wej�ciowych)
	bool pinRequired;

    //! Piny od kt�rych uzale�niony jest pin (dla pin�w wyj�ciowych)
	ReqPinsSet requiredPinsDependency;

    //! Typ pinu (wej�ciowy|wyj�ciowy)
	PinType pinType;

    //! Nazwa pinu
	std::string name;

    //! Funktor zwracaj�cy indeks pinu
     PinIndexFunc pinIndexFunc;
};

}

#endif