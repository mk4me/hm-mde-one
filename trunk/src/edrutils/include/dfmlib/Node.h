#ifndef HEADER_GUARD__NODE_H__
#define HEADER_GUARD__NODE_H__

#include <dfmlib/DFLMTypes.h>
#include <boost/enable_shared_from_this.hpp>
#include <set>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//class EditableNode;

//! Klasa reprezentujaca weze� w modelu data flow. Zarz�dza on pinami wej�ciowymi i wyj�ciowymi oraz 
class Node : public boost::enable_shared_from_this<Node>
{

public:
    //! Zaprzyja�niona klasa modelu
	friend class Model;

    //! Je�li w�ze� zezwala na edycj�
    //friend class EditableNode;

public:
    //! Typ opisuj�cy zbi�r pin�w
	//typedef std::set<PinPtr> Pins;
    typedef std::vector<PinPtr> Pins;

    //! Typ itertatora do kolekcji pin�w
    typedef Pins::const_iterator iterator;

    //! Typ rozmiaru kolekcji pin�w
    typedef Pins::size_type size_type;

protected:
    //! Interfejs klienta podczas inicjalizacji w�z�a, do dodawania pin�w (klient sam powienien trzyma� wewn�trznie wska�niki do odpowiednich pin�w)
    class IPinsAdder
    {
    public:
        //! \param pin Pin do dodania do pin�w wej�icowych
        virtual void addInPin(const PinPtr & pin) = 0;

        //! \pin Pin do dodania do pin�w wyj�ciowych
        virtual void addOutPin(const PinPtr & pin) = 0;
    };

    //! Interfejs s�u��cy do usuwania pin�w z w�z�a
    class IPinsDeleter
    {
    public:
        //! \param pin Pin do usuni�cia z pin�w wej�icowych
        virtual void removeInPin(const PinPtr & pin) = 0;

        //! \param pin Pin do usuni�cia z pin�w wyj�ciowych
        virtual void removeOutPin(const PinPtr & pin) = 0;
    };

    //! Wska�nik intelig�tny do interfejsu dodaj�cego piny do w�z�a
    typedef boost::shared_ptr<IPinsAdder> PinsAdderPtr;

    //! Wska�nik intelig�tny do interfejsu usuwaj�cego piny z w�z�a
    typedef boost::shared_ptr<IPinsDeleter> PinsDeleterPtr;

private:

    //! Prywatna, bazowa implementacja interfejsu dodaj�cego piny do w�z�a u�ywana na potrzeby inicjalizacji w�z�a
    class PinsAdder : public IPinsAdder
    {
    public:
        //! \param node W�ze� do kt�rego dodajemy piny
        PinsAdder(Node * node) : node(node) {}
        ~PinsAdder() {}

        //! Przekierowuje wywo�anie do prywatenej metody w�z�a
        virtual void addInPin(const PinPtr & pin)
        {
            if(node->initialized == true){
                throw std::runtime_error("Node was already initialized!");
            }
            node->addInPin(pin);
        }

        //! Przekierowuje wywo�anie do prywatenej metody w�z�a
        virtual void addOutPin(const PinPtr & pin)
        {
            if(node->initialized == true){
                throw std::runtime_error("Node was already initialized!");
            }
            node->addOutPin(pin);
        }

    private:
        //! W�ze� na kt�rym dzxia�amy
        Node * node;
    };

    //! Zaprzyja�nienie aby PinAdder mia� dost�p do prywatnych metod w�z�a
    friend class PinsAdder;

public:

    //! \param name nazwa w�z�a
    Node(const std::string & name = std::string());

    //! Wirtualny destruktor
	virtual ~Node(void);

    //! \return Czy w�ze� zosta� zainicjalizowany
    bool isInitialized() const;

    MPtr getModel() const;

    //! \return Iterator pocz�tku kolekcji pin�w wejsciowych
    iterator beginIn() const;

    //! \return Iterator ko�ca kolekcji pin�w wejsciowych
    iterator endIn() const;

    //! \param idx Indeks pinu wejsciowego, zg�asza wyj�tek je�li niepoprawny index
    //! \return Pin wej�ciowy spod wskazanego indeksu
    const PinPtr & getInPin(int idx) const;

    //! \param pin Pin wej�ciowy kt�rego indeks chcemy dosta�, potrzebne w przypadku zarz�dzania pinami - indeksy mog� si� zmienia�!
    //! \return Indeks pinu wej�ciowego lub -1 je�li pin wej�ciowy nie istnieje
    int indexOfInPin(const PinPtr & pin) const;

    //! \return Ilo�� pin�w wej�ciowych
    size_type sizeIn() const;

    //! \return Iterator pocz�tku kolekcji pin�w wyjsciowych
    iterator beginOut() const;

    //! \return Iterator ko�ca kolekcji pin�w wyjsciowych
    iterator endOut() const;

    //! \param idx Indeks pinu wyjsciowego, zg�asza wyj�tek je�li niepoprawny index
    //! \return Pin wyj�ciowy spod wskazanego indeksu
    const PinPtr & getOutPin(int idx) const;

    //! \param pin Pin wyj�ciowy kt�rego indeks chcemy dosta�, potrzebne w przypadku zarz�dzania pinami - indeksy mog� si� zmienia�!
    //! \return Indeks pinu wyj�ciowego lub -1 je�li pin wyj�ciowy nie istnieje
    int indexOfOutPin(const PinPtr & pin) const;

    //! \return Ilo�� pin�w wyj�ciowych
    size_type sizeOut() const;

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
	const std::string & getName() const;

    //! Metoda pozwalaj�ca podpi�� metody konfiguracji w�z�a
	virtual void configure();

protected:

    //! Inicjalizacja do implementacji przez klienta
    virtual void doInitialization(const PinsAdderPtr & pinsAdder);

    //! Resetuje w�ze� - do implementacji przez klienta
    //virtual void onReset();

private:

    //! \param name Nowa nazwa w�z�a
    void setName(const std::string & name);

    //! \param pin Pin do dodania do pin�w wejsciowych
    virtual void addInPin(const PinPtr & pin);

    //! \param pin Pin do dodania do pin�w wyjsciowych
    virtual void addOutPin(const PinPtr & pin);

    //! Inicjalizacja
    void initialize();

    //! \param pins Zbi�r pin�w w�r�d ktr�ych poszukujemy zadanego pinu
    //! \param pin Pin kt�rego indeksu poszukujemy w zadanym zbiorze
    //! \return Indeks pinu w zadanym zbiorze lub -1 je�li pinu nie znaleziono
    static int indexOfPin(const Pins & pins, const PinPtr & pin);

    //! \param pins Zbi�r pin�w do sprawdzenia
    //! \return Prawda je�li przynajmniej jeden pin jest pod��czony
	static bool anyPinConnected(const Pins & pins);
    
private:
    //! Zbi�r pin�w wej�ciowych
	Pins inPins;

    //! Zbi�r pin�w wyj�ciowych
	Pins outPins;

    //! Nazwa w�z�a
	std::string name;

    //! Czy w�ze� zainicjalizowano
    bool initialized;

    //! Model do ktorego nale�y w�ze�
    WMPtr model;   

    //! Obiekt na potrzby inicjalizacji w�z�a
    PinsAdderPtr pinsAdder; 
};

//class EditableNode : public Node
//{
//
//public:
//
//    EditableNode(const MPtr & model);
//    ~EditableNode();
//
//    MPtr getModel() const;
//
//protected:
//
//    virtual void onInPinRemove(const PinPtr & pin);
//
//    virtual void onInPinAdd(const PinPtr & pin);
//
//    virtual void onOutPinRemove(const PinPtr & pin);
//
//    virtual void onOutPinAdd(const PinPtr & pin);
//
//private:
//    //! \param newPin Pin do dodania do pin�w wejsciowych
//    void addInPin(const PinPtr & pin)
//    {
//        addPin(inPins, pin);
//        onInPinAdd(pin);
//    }
//
//    //! \param newPin Pin do dodania do pin�w wyjsciowych
//    void addOutPin(const PinPtr & pin)
//    {
//        addPin(outPins, pin);
//        onOutPinAdd(pin);
//    }
//
//    //! \param newPin Pin do dodania do pin�w wejsciowych
//    void removeInPin(const PinPtr & pin)
//    {
//        removePin(inPins, pin);
//        onInPinRemove(pin);
//    }
//
//    //! \param newPin Pin do dodania do pin�w wyjsciowych
//    void removeOutPin(const PinPtr & pin)
//    {
//        removePin(outPins, pin);
//        onOutPinRemove(pin);
//    }
//
//    static void addPin(Node::Pins & pins, const PinPtr & pin)
//    {
//        if(std::find(pins.begin(), pins.end(), pin) != pins.end()){
//            throw std::runtime_error("Pin already exists");
//        }
//
//        pins.push_back(pin);
//    }
//
//    static void removePin(Node::Pins & pins, const PinPtr & pin)
//    {
//        auto it = std::find(pins.begin(), pins.end(), pin);
//        if(it == pins.end()){
//            throw std::runtime_error("Pin not exists");
//        }
//
//        pins.erase(it);
//    }
//
//private:
//
//    WMPtr model;
//
//};

}

#endif
