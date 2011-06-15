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

//! Klasa opisujaca model logiczny data flow. Pozwala na jego edycjê - dodawanie, usuwanie wêz³ów oraz ³¹czenie/roz³anczanie pinów wg œciœle okreœlonych zasad.
class Model : public boost::enable_shared_from_this<Model>, public utils::Observable<Model>
{
public:
    //! Typ opisuj¹cy pêtle/cykle w po³aczeniach
	typedef std::vector<ConnPtr> CyclePath;

    //! Typreprezentuj¹cy zbiór wêz³ów
	typedef std::set<NPtr> Nodes;

    //! Typ opisuj¹cy zbiór po³¹czeñ
	typedef std::set<ConnPtr> Connections;

    //! Typ opisuj¹cy zbiór pinów
    typedef std::set<PinPtr> PinsSet;

    //! Typ przechowuj¹cy lokalne niezgodnoœci modelu logicznego z wytyczonymi zasadami jego tworzenia
	typedef std::map<NPtr, PinsSet > RequiringConnection;

protected:

    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

private:

    //! Struktura pomocnicza przy analizie pêtli (cyklów) w modelu
    typedef struct PathEntry{
        //! Aktualny wêze³
        NPtr node;
        //! Aktualny pin
        //Node::Pins::const_iterator pinIT;
        int pinIndex;

        //! Aktualne po³¹czenie
        //Connections::const_iterator connIT;
        int connectionIndex;

        //! Operator porównania
        bool operator==(const PathEntry & pe) const;
    }PathEntry;

public:

    //! Konstruktor zeruj¹cy
	Model(void);

    //! Wirtualny destruktor
	virtual ~Model(void);

    //! \param node Wêze³ którego kompatybilnoœæ z modelem sprawdzamy
    //! \return Czy wêze³ jest wspierany przez model
    virtual bool isNodeSupported(const NPtr & node) const;

    //! \return Czy model poprawnie skonfigurowany - pod k¹tem po³¹czeñ i wêz³ów
    bool isModelValid() const;

    //! \param node Wêze³ do dodania
    void addNode(const NPtr & node);

    //! \param node Wêze³ do usuniêcia
    void removeNode(const NPtr & node);

    //! Usuwa wszystkie wêz³y i po³¹czenia z modelu
	void clearNodes();

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Czy mo¿na po³aczyæ piny ze wzglêdu na obowi¹zuj¹ce regu³y
	bool canConnect(const CPinPtr & src, const CPinPtr & dest) const;

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Czy po³¹czenie tych pinów wprowadzi cykl (pêtle) w modelu
	bool createCycle(const CPinPtr & src, const CPinPtr & dest) const;

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Opis cyklu (œciezki) jaki powstanie przez po³¹czenie tych dwóch pinów
	CyclePath getCycle(const CPinPtr & src, const CPinPtr & dest) const;

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Po³¹czenie utworzone pomiêdzy zadanymi pinami
	ConnPtr connect(const PinPtr & src, const PinPtr & dest);

    //! \param connection Po³¹czenie do usuniêcia
    void removeConnection(const ConnPtr & connection);

    //! \param Weze³ do rozlaczenia - wszystke jego po³aczenia zostaj¹ usuniete
    void disconnectNode(const NPtr & node);

    //! Usuwa wszystkie po³¹czenia w modelu
	void clearConnections();

    //! \return Wszystkie wêz³y modelu
	const Nodes & getNodes() const;

    //! \return Wszystkie po³¹czenia w modelu
	const Connections & getConnections() const;

    //! \return Zwraca wêz³y wymagaj¹ce interwencji by model by³ poprawny
	const RequiringConnection & getRequiringConnections() const;

    //! \return Zwraca kolekcjê liœci - elementów które posiadaj¹ wejscia ale nie posiadaj¹ wyjœæ lub maj¹ wyjœcia niepod³¹czone
    const Nodes & getLeafNodes() const;

    //! \param node Wêze³ któremu nazwê zmieniamy
    //! \param name Nowa nazwa wêz³a
    void setNodeName(const NPtr & node, const std::string & name);

    //! \param pin Pin któremu nazwê zmieniamy
    //! \param name Nowa nazwa pinu
    void setPinName(const PinPtr & pin, const std::string & name);
	
 protected:
    
     //! \return true jeœli mo¿na dokonaæ zmiany modelu, inaczej false lub wyj¹tek
     virtual bool isModelChangeAllowed() const;

     //! \param src Pin Ÿród³owy (wyjœciowy)
     //! \param src Pin docelowy (wejœciowy)
     //! \return Czy mozna po³¹czyæ piny
     virtual bool additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const;

     //! \return true jeœli model jest poprawny
     virtual bool additionalModelValidation() const;

    //! \param node Wêze³ do dodania
    virtual void afterNodeAdd(const NPtr & node);

    //! \param node Wêze³ do usuniêcia
    virtual void beforeNodeRemove(const NPtr & node);

    //! \param connection Po³aczenie do usuniecia z modelu
    virtual void beforeRemoveConnection(const ConnPtr & connection);

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    virtual void afterConnect(const ConnPtr & connection);

    //! \param nodes Lista wêz³ów uznana za sprawdzone - tutaj mog¹ siê podpinaæ klasy pochodne z szersz¹ wiedz¹ na temat wêz³ów
    //! W ten sposób mog¹ przyspieszyæ wykrywanie cykli
    virtual void initCycleCheckedNodes(Nodes & nodes) const;

    //! \param node Wêze³ który sta³ siê w³aœnie liœciem
    virtual void afterLeafAdd(const NPtr & node);

    //! \param node Wêze³ który przestaje byæ liœciem
    virtual void beforeLeafRemove(const NPtr & node);

private:

    bool isLockedNonBlocking() const;

    //! \param node Wêze³ który roz³anczamy
    void quickDisconnectNode(const NPtr & node);

    //! \param connection Po³¹czenie które usuwamy
    void quickRemoveConnection(const Connections::iterator & connection);

    //! \param nodeIt Iterator wêz³a który usuwamy
    void quickRemoveNode(const Nodes::iterator & nodeIt);

    //! \param node Weze³ dla którego tworzymy strukture opisujaca kolejnosc analizowania polaczen dla sprawdzania cykli
    //! \return Aktualny stan analizowanych polaczen
    static PathEntry getFirstNodeOutputConnection(const NPtr & node);

    //! \param pathEntry Element opisujacy kolejnosc przegladania po³¹czeñ w wêŸle - jej aktualny stan
    //! \return Kolejny stan analizowanych polaczen
    static PathEntry getNextNodeOutputConnection(const PathEntry & pathEntry);

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Po³¹czenie pomiêdzy danymi pinami lub nullptr w przypadku braku po³¹czenia
    ConnPtr findConnection(CPinPtr src, CPinPtr dest) const;

    //! \param node Wêze³ który staje siê liœciem
    void addLeaf(const NPtr & node);

    //! \param node Wêze³ który przestaje byæ liœciem
    void removeLeaf(const NPtr & node);

protected:
    //! mutex dla zmiany stanu modelu
    mutable OpenThreads::Mutex editMutex;

    const Nodes & innerGetNodes() const { return nodes; }
    const Nodes & innerGetLeafNodes() const { return leafNodes; }

    const Connections & innerGetConnections() const { return connections; }

    const RequiringConnection & innerGetRequiringConnections() const { return pinsRequiringConnections; }

private:

    //! Zbiór wszystkich wezlow modelu
	Nodes nodes;

    //! Zbiór wszystkich po³¹czeñ modelu
	Connections connections;

    //! Mapa wêz³ów wymagaj¹cych interwencji aby model by³ poprawnie zbudowany
	RequiringConnection pinsRequiringConnections;

    //! Zbiór wêz³ów liœci
    Nodes leafNodes;

    //! mutex dla zmiany stanu modelu
    //mutable OpenThreads::Mutex lockMutex;
};

}

#endif
