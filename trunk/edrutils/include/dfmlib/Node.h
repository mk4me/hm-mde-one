#ifndef HEADER_GUARD__NODE_H__
#define HEADER_GUARD__NODE_H__

#include <dfmlib/DFLMTypes.h>
#include <boost/enable_shared_from_this.hpp>
#include <set>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentujaca weze� w modelu data flow. Zarz�dza on pinami wej�ciowymi i wyj�ciowymi oraz 
class Node : public boost::enable_shared_from_this<Node>
{

public:
    //! Zaprzyja�niona klasa modelu
	friend class Model;

public:
    //! Typ opisuj�cy zbi�r pin�w
	typedef std::set<PinPtr> PINS_SET;

public:

    //! \param nodeName nazwa w�z�a
    Node(const std::string & nodeName = std::string());

    //! Wirtualny destruktor
	virtual ~Node(void);

    //! \param newPin Pin do dodania do pin�w wejsciowych
    virtual void addInPin(PinPtr newPin);

    //! \param newPin Pin do dodania do pin�w wyjsciowych
    virtual void addOutPin(PinPtr newPin);

    //! \return Zbi�r pin�w wej�ciowych
	const PINS_SET & getInPins() const;

    //! \return Zbi�r pin�w wyj�ciowych
	const PINS_SET & getOutPins() const;

    //! \param node W�ze� do sprawdzenia
    //! \param pin Pin ktorego szukamy w badanym w�le
    //! \return Prawda je�li pin jest zwi�zany z badanym w�z�em, inaczej fa�sz
	static bool pinExists(const CNPtr & node, PinPtr pin);

    //! \param node W�ze� kt�remu sprawdza si� poprawno�� po��cze� pin�w wej�ciowych
    //! \return Prawda je�li po��czenia pin�w wejsciowych s� poprawne
	static bool validInPinsConnection(const CNPtr & node);

    //! \param node W�ze� kt�remu sprawdza si� poprawno�� po��cze� pin�w wyj�ciowych
    //! \return Prawda je�li po��czenia pin�w wyjsciowych s� poprawne
	static bool validOutPinsConnection(const CNPtr & node);

    //! \param node W�ze� kt�remu sprawdza si� poprawno�� pod��cze�
    //! \return Prawda je�li w�ze� jest pod��czony poprawnie
	static bool validConnection(const CNPtr & node);

    //! \param node W�ze� kt�remu sprawdza si� czy jakikolwiek pin wej�ciowy zosta� pod��czony
    //! \return Prawda je�li conajmniej jeden pin wejsciowy jest pod��czony
	static bool anyInPinConnected(const CNPtr & node);

    //! \param node W�ze� kt�remu sprawdza si� czy jakikolwiek pin wyj�ciowy zosta� pod��czony
    //! \return Prawda je�li conajmniej jeden pin wyjsciowy jest pod��czony
	static bool anyOutPinConnected(const CNPtr & node);

    //! \param node W�ze� kt�remu sprawdza si� czy jakikolwiek pin zosta� pod��czony
    //! \return Prawda je�li conajmniej jeden pin jest pod��czony
	static bool anyPinConnected(const CNPtr & node);

    //! \return Nazwa w�z�a
	const std::string & getNodeName() const;

    //! \param nodeName Nazwa w�z�a
	void setNodeName(const std::string & nodeName);

private:

    //! \param pins Zbi�r pin�w do przeszukiwania
    //! \pin Poszukiwany pin
    //! \return Prawda je�li pin istnieje w zadanym zbiorze
	static bool pinExists(const PINS_SET & pins, PinPtr pin);

    //! \param pins Zbi�r pin�w do sprawdzenia
    //! \return Prawda je�li przynajmniej jeden pin jest pod��czony
	static bool anyPinConnected(const PINS_SET & pins);
    
protected:
    //! Zbi�r pin�w wej�ciowych
	PINS_SET inPins;

    //! Zbi�r pin�w wyj�ciowych
	PINS_SET outPins;

    //! Nazwa w�z�a
	std::string nodeName;
};

}

#endif
