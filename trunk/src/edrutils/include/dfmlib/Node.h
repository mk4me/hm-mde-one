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

//! Klasa reprezentujaca weze³ w modelu data flow. Zarz¹dza on pinami wejœciowymi i wyjœciowymi oraz 
class Node : public boost::enable_shared_from_this<Node>
{

public:
    //! ZaprzyjaŸniona klasa modelu
	friend class Model;

    //! Jeœli wêze³ zezwala na edycjê
    //friend class EditableNode;

public:
    //! Typ opisuj¹cy zbiór pinów
	//typedef std::set<PinPtr> Pins;
    typedef std::vector<PinPtr> Pins;

    //! Typ itertatora do kolekcji pinów
    typedef Pins::const_iterator iterator;

    //! Typ rozmiaru kolekcji pinów
    typedef Pins::size_type size_type;

protected:
    //! Interfejs klienta podczas inicjalizacji wêz³a, do dodawania pinów (klient sam powienien trzymaæ wewnêtrznie wskaŸniki do odpowiednich pinów)
    class IPinsAdder
    {
    public:
        //! \param pin Pin do dodania do pinów wejœicowych
        virtual void addInPin(const PinPtr & pin) = 0;

        //! \pin Pin do dodania do pinów wyjœciowych
        virtual void addOutPin(const PinPtr & pin) = 0;
    };

    //! Interfejs s³u¿¹cy do usuwania pinów z wêz³a
    class IPinsDeleter
    {
    public:
        //! \param pin Pin do usuniêcia z pinów wejœicowych
        virtual void removeInPin(const PinPtr & pin) = 0;

        //! \param pin Pin do usuniêcia z pinów wyjœciowych
        virtual void removeOutPin(const PinPtr & pin) = 0;
    };

    //! WskaŸnik inteligêtny do interfejsu dodaj¹cego piny do wêz³a
    typedef boost::shared_ptr<IPinsAdder> PinsAdderPtr;

    //! WskaŸnik inteligêtny do interfejsu usuwaj¹cego piny z wêz³a
    typedef boost::shared_ptr<IPinsDeleter> PinsDeleterPtr;

private:

    //! Prywatna, bazowa implementacja interfejsu dodaj¹cego piny do wêz³a u¿ywana na potrzeby inicjalizacji wêz³a
    class PinsAdder : public IPinsAdder
    {
    public:
        //! \param node Wêze³ do którego dodajemy piny
        PinsAdder(Node * node) : node(node) {}
        ~PinsAdder() {}

        //! Przekierowuje wywo³anie do prywatenej metody wêz³a
        virtual void addInPin(const PinPtr & pin)
        {
            if(node->initialized == true){
                throw std::runtime_error("Node was already initialized!");
            }
            node->addInPin(pin);
        }

        //! Przekierowuje wywo³anie do prywatenej metody wêz³a
        virtual void addOutPin(const PinPtr & pin)
        {
            if(node->initialized == true){
                throw std::runtime_error("Node was already initialized!");
            }
            node->addOutPin(pin);
        }

    private:
        //! Wêze³ na którym dzxia³amy
        Node * node;
    };

    //! ZaprzyjaŸnienie aby PinAdder mia³ dostêp do prywatnych metod wêz³a
    friend class PinsAdder;

public:

    //! \param name nazwa wêz³a
    Node(const std::string & name = std::string());

    //! Wirtualny destruktor
	virtual ~Node(void);

    //! \return Czy wêze³ zosta³ zainicjalizowany
    bool isInitialized() const;

    MPtr getModel() const;

    //! \return Iterator pocz¹tku kolekcji pinów wejsciowych
    iterator beginIn() const;

    //! \return Iterator koñca kolekcji pinów wejsciowych
    iterator endIn() const;

    //! \param idx Indeks pinu wejsciowego, zg³asza wyj¹tek jeœli niepoprawny index
    //! \return Pin wejœciowy spod wskazanego indeksu
    const PinPtr & getInPin(int idx) const;

    //! \param pin Pin wejœciowy którego indeks chcemy dostaæ, potrzebne w przypadku zarz¹dzania pinami - indeksy mog¹ siê zmieniaæ!
    //! \return Indeks pinu wejœciowego lub -1 jeœli pin wejœciowy nie istnieje
    int indexOfInPin(const PinPtr & pin) const;

    //! \return Iloœæ pinów wejœciowych
    size_type sizeIn() const;

    //! \return Iterator pocz¹tku kolekcji pinów wyjsciowych
    iterator beginOut() const;

    //! \return Iterator koñca kolekcji pinów wyjsciowych
    iterator endOut() const;

    //! \param idx Indeks pinu wyjsciowego, zg³asza wyj¹tek jeœli niepoprawny index
    //! \return Pin wyjœciowy spod wskazanego indeksu
    const PinPtr & getOutPin(int idx) const;

    //! \param pin Pin wyjœciowy którego indeks chcemy dostaæ, potrzebne w przypadku zarz¹dzania pinami - indeksy mog¹ siê zmieniaæ!
    //! \return Indeks pinu wyjœciowego lub -1 jeœli pin wyjœciowy nie istnieje
    int indexOfOutPin(const PinPtr & pin) const;

    //! \return Iloœæ pinów wyjœciowych
    size_type sizeOut() const;

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
	const std::string & getName() const;

    //! Metoda pozwalaj¹ca podpi¹æ metody konfiguracji wêz³a
	virtual void configure();

protected:

    //! Inicjalizacja do implementacji przez klienta
    virtual void doInitialization(const PinsAdderPtr & pinsAdder);

    //! Resetuje wêze³ - do implementacji przez klienta
    //virtual void onReset();

private:

    //! \param name Nowa nazwa wêz³a
    void setName(const std::string & name);

    //! \param pin Pin do dodania do pinów wejsciowych
    virtual void addInPin(const PinPtr & pin);

    //! \param pin Pin do dodania do pinów wyjsciowych
    virtual void addOutPin(const PinPtr & pin);

    //! Inicjalizacja
    void initialize();

    //! \param pins Zbiór pinów wœród ktróych poszukujemy zadanego pinu
    //! \param pin Pin którego indeksu poszukujemy w zadanym zbiorze
    //! \return Indeks pinu w zadanym zbiorze lub -1 jeœli pinu nie znaleziono
    static int indexOfPin(const Pins & pins, const PinPtr & pin);

    //! \param pins Zbiór pinów do sprawdzenia
    //! \return Prawda jeœli przynajmniej jeden pin jest pod³¹czony
	static bool anyPinConnected(const Pins & pins);
    
private:
    //! Zbiór pinów wejœciowych
	Pins inPins;

    //! Zbiór pinów wyjœciowych
	Pins outPins;

    //! Nazwa wêz³a
	std::string name;

    //! Czy wêze³ zainicjalizowano
    bool initialized;

    //! Model do ktorego nale¿y wêze³
    WMPtr model;   

    //! Obiekt na potrzby inicjalizacji wêz³a
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
//    //! \param newPin Pin do dodania do pinów wejsciowych
//    void addInPin(const PinPtr & pin)
//    {
//        addPin(inPins, pin);
//        onInPinAdd(pin);
//    }
//
//    //! \param newPin Pin do dodania do pinów wyjsciowych
//    void addOutPin(const PinPtr & pin)
//    {
//        addPin(outPins, pin);
//        onOutPinAdd(pin);
//    }
//
//    //! \param newPin Pin do dodania do pinów wejsciowych
//    void removeInPin(const PinPtr & pin)
//    {
//        removePin(inPins, pin);
//        onInPinRemove(pin);
//    }
//
//    //! \param newPin Pin do dodania do pinów wyjsciowych
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
