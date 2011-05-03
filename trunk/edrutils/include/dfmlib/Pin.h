#ifndef HEADER_GUARD__PIN_H__
#define HEADER_GUARD__PIN_H__

#include <dfmlib/DFLMTypes.h>
#include <set>
#include <boost/enable_shared_from_this.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentuj�ca punkt ��czenia w�z��w. Rozr�niamy 2 typy pin�w - wej�ciowe i wyj�ciowe.
class Pin : public boost::enable_shared_from_this<Pin>
{
public:

    //! Typ okre�laj�cy typ pinu
	typedef enum {  PIN_IN, //! Pin wejsciowy
                    PIN_OUT, //! Pin wyj�ciowy
                    PIN_UNKNOWN //! Pin niezdefiniowany
    } PIN_TYPE;

    //! Typ okre�laj�cy statyczny stan pinu
	typedef enum {  PIN_OK,         //! Z pinem wszystko OK, neutralny stan pinu niepod��czonego bez �adnych wymaga� (zale�no�ci, wymaganie pod�aczenia)
                    PIN_REQUIRED,   //! Pin jest wymagany a nie jest pod��czony (dotyczy pin�w wej�ciowych)
                    PIN_INCOMPLETE, //! Pin ma niespe�nione zale�no�ci - jeden z pin�w od kt�rych jest zale�ny nie jest pod��czony (dotyczy pin�w wyj�ciowych)
                    PIN_CONNECTED   //! Pin poprawnie pod��czony
    } PIN_STATIC_STATUS;

    //! Typ okre�laj�cy dynamiczny stan pinu podczas ��czenia
	typedef enum {  PIN_COMPATIBLE,         //! Pin kompatybilny - mo�na ��czy�
                    PIN_COMPATIBLE_CYCLE,   //! Pin kompatybilny ale tworzy cykl - nie mo�na ��czy�
                    PIN_COMPATIBLE_FULL,    //! Pin kompatybilny ale docelowy pin wej�ciowy ju� zaj�ty - nie mo�na ��czy�
                    PIN_INCOMPATIBLE        //! Pin nmiekompatybilny - nie mo�na ��czy�
    } PIN_DYNAMIC_STATUS;

    //! Typ kolekcji po��cze�
	typedef std::set<ConnPtr> CONNECTIONS_SET;

    //! Typ kolekcji pion�w wymaganych
	typedef std::set<WPinPtr> REQ_PINS_SET;

public:

    //! Zaprzyja�niony typ w�z�a - one zarz�dzaj� swoimi pinami i na ich bazie s� �aczone
	friend class Node;

    //! Zaprzyja�niony typ modelu - zarz�dza, weryfikuje stan modelu, regu�y jego ��czenia mi�dzy innymi przez piny
	friend class Model;

public:

    //! Konstruktor domy�lny
    //! \param pinName nazwa pinu
    //! \param required Czy pin wymagany (dotyczy tylko pin�w wejsciowych)
    //! \param requiredPins Piny od kt�rych jest zale�ny aktualny pin (dotyczy pin�w wyj�ciowych)
	Pin(const std::string & pinName = std::string(),
		bool required = false, const REQ_PINS_SET & requiredPins = REQ_PINS_SET());
	
    //! Wirtualny destruktor
	virtual ~Pin(void);

    //! \return Wska�nik do rodzica lub nullptr je�li pin nie jest dodany do w�z�a
	NPtr getParent() const;

    //! \return Zbi�r po��cze� zwi�zanych z tym pinem
	const CONNECTIONS_SET & getConnections() const;

    //! \return Typ pinu (IN|OUT)
	PIN_TYPE getType() const;

    //! \return Statyczny status pinu
	PIN_STATIC_STATUS getStaticStatus() const;

    //! \param refPin Pin wzgledem ktorego badamy stan dynamiczny pinu (podczas ��czenia)
    //! \param model Model w obr�bie kt�rego si� poruszamy definiuj�c status dynamiczny
    //! \return Status dynamiczny pinu przed prob� po��czenia
	PIN_DYNAMIC_STATUS getDynamicStatus(PinPtr refPin, MPtr model) const;

    //! \param pin Pin kt�rego kompatybilno�c testujemy
    //! \return Prawda je�li piny s� kompatybilne (aktualny pin potrafi obs�u�y� testowany pin)
	virtual bool isCompatible(CPinPtr pin) const;
	
    //! \param pin Pin kt�rego testujemy czy aktualny pin jest od niego zale�ny (dotyczy pin�w wyjsciowych)
    //! \returen Prawda je�li aktualny pin jest zale�ny od zadanego pinu
	bool dependsOnPin(PinPtr pin) const;

    //! \return Zwraca zbi�r pin�w od kt�rych jest uzale�niony aktualny pin
	const REQ_PINS_SET & getDependantPins() const;

    //! \return Czy dany pin jest wymagany (dotyczy pin�w wej�ciowych)
	bool isRequired() const;

    //! \return Czy wszystkie zale�no�ci pinu s� spe�nione (dotyczy pin�w wyj�ciowych)
	bool isComplete() const;

    //! \return Nazwa pinu
	const std::string & getPinName() const;

    //! \param Nazwa pinu
	void setPinName(const std::string & pinName);

private:

    //! \param connection Po��czenie dodawane do pinu
    void addConnection(ConnPtr connection);

    //! \param connection Po��czenie usuwane z pinu
    void removeConnection(ConnPtr connection);

    //! Usuwa wszystkie po��czenia pinu
	void clearConnections();

    //! type Typ pinu ustawiany podczas do�anczania go do w�z�a
	void setType(PIN_TYPE type);

private:

    //! Zbi�r po��cze� pinu
	CONNECTIONS_SET connections;

    //! W�ze� rodzic pinu
	WNPtr parentNode;

    //! Czy pin wymagany (dla pin �w wej�ciowych)
	bool pinRequired;

    //! Piny od kt�rych uzale�niony jest pin (dla pin�w wyj�ciowych)
	REQ_PINS_SET requiredPinsDependency;

    //! Typ pinu (wej�ciowy|wyj�ciowy)
	PIN_TYPE pinType;

    //! Nazwa pinu
	std::string pinName;
};

}

#endif