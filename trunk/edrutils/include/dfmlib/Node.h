#ifndef HEADER_GUARD__NODE_H__
#define HEADER_GUARD__NODE_H__

#include <dfmlib/DFLMTypes.h>
#include <boost/enable_shared_from_this.hpp>
#include <set>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentujaca weze³ w modelu data flow. Zarz¹dza on pinami wejœciowymi i wyjœciowymi oraz 
class Node : public boost::enable_shared_from_this<Node>
{

public:
    //! ZaprzyjaŸniona klasa modelu
	friend class Model;

public:
    //! Typ opisuj¹cy zbiór pinów
	typedef std::set<PinPtr> PINS_SET;

public:

    //! \param nodeName nazwa wêz³a
    Node(const std::string & nodeName = std::string());

    //! Wirtualny destruktor
	virtual ~Node(void);

    //! \param newPin Pin do dodania do pinów wejsciowych
    virtual void addInPin(PinPtr newPin);

    //! \param newPin Pin do dodania do pinów wyjsciowych
    virtual void addOutPin(PinPtr newPin);

    //! \return Zbiór pinów wejœciowych
	const PINS_SET & getInPins() const;

    //! \return Zbiór pinów wyjœciowych
	const PINS_SET & getOutPins() const;

    //! \param node Wêze³ do sprawdzenia
    //! \param pin Pin ktorego szukamy w badanym wêŸle
    //! \return Prawda jeœli pin jest zwi¹zany z badanym wêz³em, inaczej fa³sz
	static bool pinExists(const CNPtr & node, PinPtr pin);

    //! \param node Wêze³ któremu sprawdza siê poprawnoœæ po³¹czeñ pinów wejœciowych
    //! \return Prawda jeœli po³¹czenia pinów wejsciowych s¹ poprawne
	static bool validInPinsConnection(const CNPtr & node);

    //! \param node Wêze³ któremu sprawdza siê poprawnoœæ po³¹czeñ pinów wyjœciowych
    //! \return Prawda jeœli po³¹czenia pinów wyjsciowych s¹ poprawne
	static bool validOutPinsConnection(const CNPtr & node);

    //! \param node Wêze³ któremu sprawdza siê poprawnoœæ pod³¹czeñ
    //! \return Prawda jeœli wêze³ jest pod³¹czony poprawnie
	static bool validConnection(const CNPtr & node);

    //! \param node Wêze³ któremu sprawdza siê czy jakikolwiek pin wejœciowy zosta³ pod³¹czony
    //! \return Prawda jeœli conajmniej jeden pin wejsciowy jest pod³¹czony
	static bool anyInPinConnected(const CNPtr & node);

    //! \param node Wêze³ któremu sprawdza siê czy jakikolwiek pin wyjœciowy zosta³ pod³¹czony
    //! \return Prawda jeœli conajmniej jeden pin wyjsciowy jest pod³¹czony
	static bool anyOutPinConnected(const CNPtr & node);

    //! \param node Wêze³ któremu sprawdza siê czy jakikolwiek pin zosta³ pod³¹czony
    //! \return Prawda jeœli conajmniej jeden pin jest pod³¹czony
	static bool anyPinConnected(const CNPtr & node);

    //! \return Nazwa wêz³a
	const std::string & getNodeName() const;

    //! \param nodeName Nazwa wêz³a
	void setNodeName(const std::string & nodeName);

private:

    //! \param pins Zbiór pinów do przeszukiwania
    //! \pin Poszukiwany pin
    //! \return Prawda jeœli pin istnieje w zadanym zbiorze
	static bool pinExists(const PINS_SET & pins, PinPtr pin);

    //! \param pins Zbiór pinów do sprawdzenia
    //! \return Prawda jeœli przynajmniej jeden pin jest pod³¹czony
	static bool anyPinConnected(const PINS_SET & pins);
    
protected:
    //! Zbiór pinów wejœciowych
	PINS_SET inPins;

    //! Zbiór pinów wyjœciowych
	PINS_SET outPins;

    //! Nazwa wêz³a
	std::string nodeName;
};

}

#endif
