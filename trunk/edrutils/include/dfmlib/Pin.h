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

//! Klasa reprezentuj¹ca punkt ³¹czenia wêz³ów. Rozró¿niamy 2 typy pinów - wejœciowe i wyjœciowe.
class Pin : public boost::enable_shared_from_this<Pin>
{
public:

    //! Typ okreœlaj¹cy typ pinu
	enum PinType {
        IN, //! Pin wejsciowy
        OUT, //! Pin wyjœciowy
        UNSET //! Pin niezdefiniowany
    };

    //! Typ okreœlaj¹cy statyczny stan pinu
	enum StaticStatus {
        OK,         //! Z pinem wszystko OK, neutralny stan pinu niepod³¹czonego bez ¿adnych wymagañ (zale¿noœci, wymaganie pod³aczenia)
        REQUIRED,   //! Pin jest wymagany a nie jest pod³¹czony (dotyczy pinów wejœciowych)
        INCOMPLETE, //! Pin ma niespe³nione zale¿noœci - jeden z pinów od których jest zale¿ny nie jest pod³¹czony (dotyczy pinów wyjœciowych)
        CONNECTED   //! Pin poprawnie pod³¹czony
    };

    //! Typ okreœlaj¹cy dynamiczny stan pinu podczas ³¹czenia
	enum DynamicStatus {
        COMPATIBLE,         //! Pin kompatybilny - mo¿na ³¹czyæ
        COMPATIBLE_CYCLE,   //! Pin kompatybilny ale tworzy cykl - nie mo¿na ³¹czyæ
        COMPATIBLE_FULL,    //! Pin kompatybilny ale docelowy pin wejœciowy ju¿ zajêty - nie mo¿na ³¹czyæ
        INCOMPATIBLE        //! Pin nmiekompatybilny - nie mo¿na ³¹czyæ
    };

    //! Typ kolekcji po³¹czeñ
	//typedef std::set<ConnPtr> Connections;
    typedef std::vector<ConnPtr> Connections;

    //! Typ kolekcji pionów wymaganych
	typedef std::set<WPinPtr> ReqPinsSet;

    //! Typ iteratora do kolekcji po³¹czeñ
    typedef Connections::const_iterator iterator;

    //! Typ iloœci po³¹czeñ w kolekcji
    typedef Connections::size_type size_type;

private:

    //! Funktor zwracaj¹cy indeks pinu
    typedef boost::function<int(const PinPtr &)> PinIndexFunc;

public:

    //! ZaprzyjaŸniony typ wêz³a - one zarz¹dzaj¹ swoimi pinami i na ich bazie s¹ ³aczone
	friend class Node;

    //! ZaprzyjaŸniony typ modelu - zarz¹dza, weryfikuje stan modelu, regu³y jego ³¹czenia miêdzy innymi przez piny
	friend class Model;

public:

    //! Konstruktor domyœlny
    //! \param name nazwa pinu
    //! \param required Czy pin wymagany (dotyczy tylko pinów wejsciowych)
    //! \param requiredPins Piny od których jest zale¿ny aktualny pin (dotyczy pinów wyjœciowych)
	Pin(const std::string & name = std::string(),
		bool required = false, const ReqPinsSet & requiredPins = ReqPinsSet());
	
    //! Wirtualny destruktor
	virtual ~Pin(void);

    //! \return WskaŸnik do rodzica lub nullptr jeœli pin nie jest dodany do wêz³a
	NPtr getParent() const;

    //! \return Iterator pocz¹tka kolekcji po³¹czeñ
    iterator begin() const;

    //! \return Iterator koñca kolekcji po³¹czeñ
    iterator end() const;

    //! \param idx Indeks po³¹czenia
    //! \return Zwraca po³aczenie o zadanym indeksie
    const ConnPtr & getConnection(int idx) const;

    //! \param connection Po³aczenie którego indeksu szukamy
    //! \return Indeks po³¹czenia w kolekcji lub -1 jeœli po³¹czenia nie ma w kolekcji
    int getIndexOfConnection(const ConnPtr & connection) const;

    //! \return Czy kolekcja po³¹czeñ jest pusta
    bool empty() const;

    //! \return Iloœæ po³¹czeñ w kolekcji
    size_type size() const;

    //! \return Typ pinu (IN|OUT)
	PinType getType() const;

    //! \return Statyczny status pinu
	StaticStatus getStaticStatus() const;

    //! \param refPin Pin wzgledem ktorego badamy stan dynamiczny pinu (podczas ³¹czenia)
    //! \param model Model w obrêbie którego siê poruszamy definiuj¹c status dynamiczny
    //! \return Status dynamiczny pinu przed prob¹ po³¹czenia
	DynamicStatus getDynamicStatus(const PinPtr & refPin, const MPtr & model) const;

    //! \param pin Pin którego kompatybilnoœc testujemy
    //! \return Prawda jeœli piny s¹ kompatybilne (aktualny pin potrafi obs³u¿yæ testowany pin)
	virtual bool isCompatible(const CPinPtr & pin) const;
	
    //! \param pin Pin którego testujemy czy aktualny pin jest od niego zale¿ny (dotyczy pinów wyjsciowych)
    //! \returen Prawda jeœli aktualny pin jest zale¿ny od zadanego pinu
	bool dependsOnPin(const PinPtr & pin) const;

    //! \return Zwraca zbiór pinów od których jest uzale¿niony aktualny pin
	const ReqPinsSet & getDependantPins() const;

    //! \return Czy dany pin jest wymagany (dotyczy pinów wejœciowych)
	bool isRequired() const;

    //! \return Czy wszystkie zale¿noœci pinu s¹ spe³nione (dotyczy pinów wyjœciowych)
	bool isComplete() const;

    //! \return Nazwa pinu
	const std::string & getName() const;

 //   //! \param Nazwa pinu
	//void setPinName(const std::string & name);

    //! \return Indeks pinu w wêŸle (-1 jeœli pin nigdzie nie dodany, automatycznie ropoznaje typ pinu
    int getPinIndex() const;

private:

    //! \param name Nowa nazwa pinu
    void setName(const std::string & name);

    //! \param connection Po³¹czenie dodawane do pinu
    void addConnection(const ConnPtr & connection);

    //! \param connection Po³¹czenie usuwane z pinu
    void removeConnection(const ConnPtr & connection);

    //! Usuwa wszystkie po³¹czenia pinu
	void clearConnections();

    //! \param type Typ pinu ustawiany podczas do³anczania go do wêz³a
	void setType(PinType type);

private:

    //! Zbiór po³¹czeñ pinu
	Connections connections;

    //! Wêze³ rodzic pinu
	WNPtr parentNode;

    //! Czy pin wymagany (dla pin ów wejœciowych)
	bool pinRequired;

    //! Piny od których uzale¿niony jest pin (dla pinów wyjœciowych)
	ReqPinsSet requiredPinsDependency;

    //! Typ pinu (wejœciowy|wyjœciowy)
	PinType pinType;

    //! Nazwa pinu
	std::string name;

    //! Funktor zwracaj¹cy indeks pinu
     PinIndexFunc pinIndexFunc;
};

}

#endif