#ifndef HEADER_GUARD__PIN_H__
#define HEADER_GUARD__PIN_H__

#include <dfmlib/DFLMTypes.h>
#include <set>
#include <boost/enable_shared_from_this.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentuj¹ca punkt ³¹czenia wêz³ów. Rozró¿niamy 2 typy pinów - wejœciowe i wyjœciowe.
class Pin : public boost::enable_shared_from_this<Pin>
{
public:

    //! Typ okreœlaj¹cy typ pinu
	typedef enum {  PIN_IN, //! Pin wejsciowy
                    PIN_OUT, //! Pin wyjœciowy
                    PIN_UNKNOWN //! Pin niezdefiniowany
    } PIN_TYPE;

    //! Typ okreœlaj¹cy statyczny stan pinu
	typedef enum {  PIN_OK,         //! Z pinem wszystko OK, neutralny stan pinu niepod³¹czonego bez ¿adnych wymagañ (zale¿noœci, wymaganie pod³aczenia)
                    PIN_REQUIRED,   //! Pin jest wymagany a nie jest pod³¹czony (dotyczy pinów wejœciowych)
                    PIN_INCOMPLETE, //! Pin ma niespe³nione zale¿noœci - jeden z pinów od których jest zale¿ny nie jest pod³¹czony (dotyczy pinów wyjœciowych)
                    PIN_CONNECTED   //! Pin poprawnie pod³¹czony
    } PIN_STATIC_STATUS;

    //! Typ okreœlaj¹cy dynamiczny stan pinu podczas ³¹czenia
	typedef enum {  PIN_COMPATIBLE,         //! Pin kompatybilny - mo¿na ³¹czyæ
                    PIN_COMPATIBLE_CYCLE,   //! Pin kompatybilny ale tworzy cykl - nie mo¿na ³¹czyæ
                    PIN_COMPATIBLE_FULL,    //! Pin kompatybilny ale docelowy pin wejœciowy ju¿ zajêty - nie mo¿na ³¹czyæ
                    PIN_INCOMPATIBLE        //! Pin nmiekompatybilny - nie mo¿na ³¹czyæ
    } PIN_DYNAMIC_STATUS;

    //! Typ kolekcji po³¹czeñ
	typedef std::set<ConnPtr> CONNECTIONS_SET;

    //! Typ kolekcji pionów wymaganych
	typedef std::set<WPinPtr> REQ_PINS_SET;

public:

    //! ZaprzyjaŸniony typ wêz³a - one zarz¹dzaj¹ swoimi pinami i na ich bazie s¹ ³aczone
	friend class Node;

    //! ZaprzyjaŸniony typ modelu - zarz¹dza, weryfikuje stan modelu, regu³y jego ³¹czenia miêdzy innymi przez piny
	friend class Model;

public:

    //! Konstruktor domyœlny
    //! \param pinName nazwa pinu
    //! \param required Czy pin wymagany (dotyczy tylko pinów wejsciowych)
    //! \param requiredPins Piny od których jest zale¿ny aktualny pin (dotyczy pinów wyjœciowych)
	Pin(const std::string & pinName = std::string(),
		bool required = false, const REQ_PINS_SET & requiredPins = REQ_PINS_SET());
	
    //! Wirtualny destruktor
	virtual ~Pin(void);

    //! \return WskaŸnik do rodzica lub nullptr jeœli pin nie jest dodany do wêz³a
	NPtr getParent() const;

    //! \return Zbiór po³¹czeñ zwi¹zanych z tym pinem
	const CONNECTIONS_SET & getConnections() const;

    //! \return Typ pinu (IN|OUT)
	PIN_TYPE getType() const;

    //! \return Statyczny status pinu
	PIN_STATIC_STATUS getStaticStatus() const;

    //! \param refPin Pin wzgledem ktorego badamy stan dynamiczny pinu (podczas ³¹czenia)
    //! \param model Model w obrêbie którego siê poruszamy definiuj¹c status dynamiczny
    //! \return Status dynamiczny pinu przed prob¹ po³¹czenia
	PIN_DYNAMIC_STATUS getDynamicStatus(PinPtr refPin, MPtr model) const;

    //! \param pin Pin którego kompatybilnoœc testujemy
    //! \return Prawda jeœli piny s¹ kompatybilne (aktualny pin potrafi obs³u¿yæ testowany pin)
	virtual bool isCompatible(CPinPtr pin) const;
	
    //! \param pin Pin którego testujemy czy aktualny pin jest od niego zale¿ny (dotyczy pinów wyjsciowych)
    //! \returen Prawda jeœli aktualny pin jest zale¿ny od zadanego pinu
	bool dependsOnPin(PinPtr pin) const;

    //! \return Zwraca zbiór pinów od których jest uzale¿niony aktualny pin
	const REQ_PINS_SET & getDependantPins() const;

    //! \return Czy dany pin jest wymagany (dotyczy pinów wejœciowych)
	bool isRequired() const;

    //! \return Czy wszystkie zale¿noœci pinu s¹ spe³nione (dotyczy pinów wyjœciowych)
	bool isComplete() const;

    //! \return Nazwa pinu
	const std::string & getPinName() const;

    //! \param Nazwa pinu
	void setPinName(const std::string & pinName);

private:

    //! \param connection Po³¹czenie dodawane do pinu
    void addConnection(ConnPtr connection);

    //! \param connection Po³¹czenie usuwane z pinu
    void removeConnection(ConnPtr connection);

    //! Usuwa wszystkie po³¹czenia pinu
	void clearConnections();

    //! type Typ pinu ustawiany podczas do³anczania go do wêz³a
	void setType(PIN_TYPE type);

private:

    //! Zbiór po³¹czeñ pinu
	CONNECTIONS_SET connections;

    //! Wêze³ rodzic pinu
	WNPtr parentNode;

    //! Czy pin wymagany (dla pin ów wejœciowych)
	bool pinRequired;

    //! Piny od których uzale¿niony jest pin (dla pinów wyjœciowych)
	REQ_PINS_SET requiredPinsDependency;

    //! Typ pinu (wejœciowy|wyjœciowy)
	PIN_TYPE pinType;

    //! Nazwa pinu
	std::string pinName;
};

}

#endif