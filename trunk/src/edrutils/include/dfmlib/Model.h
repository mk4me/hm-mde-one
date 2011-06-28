#ifndef HEADER_GUARD__MODEL_H__
#define HEADER_GUARD__MODEL_H__

#include <utils/Synchronized.h>
#include <utils/ObserverPattern.h>
#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <boost/enable_shared_from_this.hpp>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <set>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa opisujaca model logiczny data flow. Pozwala na jego edycj� - dodawanie, usuwanie w�z��w oraz ��czenie/roz�anczanie pin�w wg �ci�le okre�lonych zasad.
class Model : public boost::enable_shared_from_this<Model>, public utils::Observable<Model>
{
public:
    //! Typ opisuj�cy p�tle/cykle w po�aczeniach
	typedef std::vector<ConnPtr> CyclePath;

    //! Typreprezentuj�cy zbi�r w�z��w
	typedef std::set<NPtr> Nodes;

    //! Typ opisuj�cy zbi�r po��cze�
	typedef std::set<ConnPtr> Connections;

    //! Typ opisuj�cy zbi�r pin�w
    typedef std::set<PinPtr> PinsSet;

    //! Typ przechowuj�cy lokalne niezgodno�ci modelu logicznego z wytyczonymi zasadami jego tworzenia
	typedef std::map<NPtr, PinsSet > RequiringConnection;

protected:

    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

private:

    //! Struktura pomocnicza przy analizie p�tli (cykl�w) w modelu
    typedef struct PathEntry{
        //! Aktualny w�ze�
        NPtr node;
        //! Aktualny pin
        //Node::Pins::const_iterator pinIT;
        int pinIndex;

        //! Aktualne po��czenie
        //Connections::const_iterator connIT;
        int connectionIndex;

        //! Operator por�wnania
        bool operator==(const PathEntry & pe) const;
    }PathEntry;

public:

    //! Konstruktor zeruj�cy
	Model(void);

    //! Wirtualny destruktor
	virtual ~Model(void);

    //! \param node W�ze� kt�rego kompatybilno�� z modelem sprawdzamy
    //! \return Czy w�ze� jest wspierany przez model
    virtual bool isNodeSupported(const NPtr & node) const;

    //! \return Czy model poprawnie skonfigurowany - pod k�tem po��cze� i w�z��w
    bool isModelValid() const;

    //! \param node W�ze� do dodania
    void addNode(const NPtr & node);

    //! \param node W�ze� do usuni�cia
    void removeNode(const NPtr & node);

    //! Usuwa wszystkie w�z�y i po��czenia z modelu
	void clearNodes();

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Czy mo�na po�aczy� piny ze wzgl�du na obowi�zuj�ce regu�y
	bool canConnect(const CPinPtr & src, const CPinPtr & dest) const;

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Czy po��czenie tych pin�w wprowadzi cykl (p�tle) w modelu
	bool createCycle(const CPinPtr & src, const CPinPtr & dest) const;

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Opis cyklu (�ciezki) jaki powstanie przez po��czenie tych dw�ch pin�w
	CyclePath getCycle(const CPinPtr & src, const CPinPtr & dest) const;

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Po��czenie utworzone pomi�dzy zadanymi pinami
	ConnPtr connect(const PinPtr & src, const PinPtr & dest);

    //! \param connection Po��czenie do usuni�cia
    void removeConnection(const ConnPtr & connection);

    //! \param Weze� do rozlaczenia - wszystke jego po�aczenia zostaj� usuniete
    void disconnectNode(const NPtr & node);

    //! Usuwa wszystkie po��czenia w modelu
	void clearConnections();

    //! \return Wszystkie w�z�y modelu
	const Nodes & getNodes() const;

    //! \return Wszystkie po��czenia w modelu
	const Connections & getConnections() const;

    //! \return Zwraca w�z�y wymagaj�ce interwencji by model by� poprawny
	const RequiringConnection & getRequiringConnections() const;

    //! \return Zwraca kolekcj� li�ci - element�w kt�re posiadaj� wejscia ale nie posiadaj� wyj�� lub maj� wyj�cia niepod��czone
    const Nodes & getLeafNodes() const;

    //! \param node W�ze� kt�remu nazw� zmieniamy
    //! \param name Nowa nazwa w�z�a
    void setNodeName(const NPtr & node, const std::string & name);

    //! \param pin Pin kt�remu nazw� zmieniamy
    //! \param name Nowa nazwa pinu
    void setPinName(const PinPtr & pin, const std::string & name);
	
 protected:
    
     //! \return true je�li mo�na dokona� zmiany modelu, inaczej false lub wyj�tek
     virtual bool isModelChangeAllowed() const;

     //! \param src Pin �r�d�owy (wyj�ciowy)
     //! \param src Pin docelowy (wej�ciowy)
     //! \return Czy mozna po��czy� piny
     virtual bool additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const;

     //! \return true je�li model jest poprawny
     virtual bool additionalModelValidation() const;

    //! \param node W�ze� do dodania
    virtual void afterNodeAdd(const NPtr & node);

    //! \param node W�ze� do usuni�cia
    virtual void beforeNodeRemove(const NPtr & node);

    //! \param connection Po�aczenie do usuniecia z modelu
    virtual void beforeRemoveConnection(const ConnPtr & connection);

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    virtual void afterConnect(const ConnPtr & connection);

    //! \param nodes Lista w�z��w uznana za sprawdzone - tutaj mog� si� podpina� klasy pochodne z szersz� wiedz� na temat w�z��w
    //! W ten spos�b mog� przyspieszy� wykrywanie cykli
    virtual void initCycleCheckedNodes(Nodes & nodes) const;

    //! \param node W�ze� kt�ry sta� si� w�a�nie li�ciem
    virtual void afterLeafAdd(const NPtr & node);

    //! \param node W�ze� kt�ry przestaje by� li�ciem
    virtual void beforeLeafRemove(const NPtr & node);

private:

    bool isLockedNonBlocking() const;

    //! \param node W�ze� kt�ry roz�anczamy
    void quickDisconnectNode(const NPtr & node);

    //! \param connection Po��czenie kt�re usuwamy
    void quickRemoveConnection(const Connections::iterator & connection);

    //! \param nodeIt Iterator w�z�a kt�ry usuwamy
    void quickRemoveNode(const Nodes::iterator & nodeIt);

    //! \param node Weze� dla kt�rego tworzymy strukture opisujaca kolejnosc analizowania polaczen dla sprawdzania cykli
    //! \return Aktualny stan analizowanych polaczen
    static PathEntry getFirstNodeOutputConnection(const NPtr & node);

    //! \param pathEntry Element opisujacy kolejnosc przegladania po��cze� w w�le - jej aktualny stan
    //! \return Kolejny stan analizowanych polaczen
    static PathEntry getNextNodeOutputConnection(const PathEntry & pathEntry);

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Po��czenie pomi�dzy danymi pinami lub nullptr w przypadku braku po��czenia
    ConnPtr findConnection(CPinPtr src, CPinPtr dest) const;

    //! \param node W�ze� kt�ry staje si� li�ciem
    void addLeaf(const NPtr & node);

    //! \param node W�ze� kt�ry przestaje by� li�ciem
    void removeLeaf(const NPtr & node);

protected:
    //! mutex dla zmiany stanu modelu
    mutable OpenThreads::Mutex editMutex;

    const Nodes & innerGetNodes() const { return nodes; }
    const Nodes & innerGetLeafNodes() const { return leafNodes; }

    const Connections & innerGetConnections() const { return connections; }

    const RequiringConnection & innerGetRequiringConnections() const { return pinsRequiringConnections; }

private:

    //! Zbi�r wszystkich wezlow modelu
	Nodes nodes;

    //! Zbi�r wszystkich po��cze� modelu
	Connections connections;

    //! Mapa w�z��w wymagaj�cych interwencji aby model by� poprawnie zbudowany
	RequiringConnection pinsRequiringConnections;

    //! Zbi�r w�z��w li�ci
    Nodes leafNodes;

    //! mutex dla zmiany stanu modelu
    //mutable OpenThreads::Mutex lockMutex;
};

}

#endif
