#ifndef HEADER_GUARD__MODEL_H__
#define HEADER_GUARD__MODEL_H__

#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <boost/enable_shared_from_this.hpp>
#include <set>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa opisujaca model logiczny data flow. Pozwala na jego edycjê - dodawanie, usuwanie wêz³ów oraz ³¹czenie/roz³anczanie pinów wg œciœle okreœlonych zasad.
class Model : public boost::enable_shared_from_this<Model>
{
public:
    //! Typ opisuj¹cy pêtle/cykle w po³aczeniach
	typedef std::vector<ConnPtr> CYCLE;

    //! Typreprezentuj¹cy zbiór wêz³ów
	typedef std::set<NPtr> NODES_SET;

    //! Typ opisuj¹cy zbiór po³¹czeñ
	typedef Pin::CONNECTIONS_SET CONNECTIONS_SET;

private:

    //! Struktura pomocnicza przy analizie pêtli (cyklów) w modelu
    typedef struct MY_PATH_ENTRY{
        //! Aktualny wêze³
        NPtr node;
        //! Aktualny pin
        Node::PINS_SET::const_iterator pinIT;

        //! Aktualne po³¹czenie
        CONNECTIONS_SET::const_iterator connIT;

        //! Operator porównania
        bool operator==(const MY_PATH_ENTRY & pe) const;
    }PATH_ENTRY;

public:

    //! Konstruktor zeruj¹cy
	Model(void);

    //! Wirtualny destruktor
	virtual ~Model(void);

    //! \param node Wêze³ do dodania
    virtual void addNode(NPtr node);

    //! \param node Wêze³ do usuniêcia
    virtual void removeNode(NPtr node);

    //! Usuwa wszystkie wêz³y i po³¹czenia z modelu
	virtual void clearNodes();

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Czy mo¿na po³aczyæ piny ze wzglêdu na obowi¹zuj¹ce regu³y
	virtual bool canConnect(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Czy po³¹czenie tych pinów wprowadzi cykl (pêtle) w modelu
	bool createCycle(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Opis cyklu (œciezki) jaki powstanie przez po³¹czenie tych dwóch pinów
	virtual CYCLE getCycle(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Po³¹czenie utworzone pomiêdzy zadanymi pinami
	virtual ConnPtr connect(PinPtr src, PinPtr dest);

    //! \param connection Po³¹czenie do usuniêcia
    virtual void removeConnection(ConnPtr connection);

    //! Usuwa wszystkie po³¹czenia w modelu
	virtual void clearConnections();

    //! \return Wszystkie wêz³y modelu
	const NODES_SET & getNodes() const;

    //! \return Wszystkie po³¹czenia w modelu
	const CONNECTIONS_SET & getConnections() const;
	
 protected:

     static void unregisterPin(PinPtr pin);
    
     //! \param Weze³ do rozlaczenia - wszystke jego po³aczenia zostaj¹ usuniete
     virtual void disconnectNode(NPtr node);
    
    //! \param connection Po³aczenie do usuniecia z modelu
     virtual void quickRemoveConnection(ConnPtr connection);

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Po³aczenie miedzy pinami
	virtual ConnPtr quickConnect(PinPtr src, PinPtr dest);

    //! \param nodes Lista wêz³ów uznana za sprawdzone - tutaj mog¹ siê podpinaæ klasy pochodne z szersz¹ wiedz¹ na temat wêz³ów
    //! W ten sposób mog¹ przyspieszyæ wykrywanie cykli
    virtual void initCheckedNodes(NODES_SET & nodes) const;

private:
    //! \param node Weze³ dla którego tworzymy strukture opisujaca kolejnosc analizowania polaczen dla sprawdzania cykli
    //! \return Aktualny stan analizowanych polaczen
    static PATH_ENTRY getFirstNodeOutputConnection(NPtr node);

    //! \param pathElement Element opisujacy kolejnosc przegladania po³¹czeñ w wêŸle - jej aktualny stan
    //! \return Kolejny stan analizowanych polaczen
    static PATH_ENTRY getNextNodeOutputConnection(const PATH_ENTRY & pathElement);

    //! \param src Pin Ÿród³owy (wyjœciowy)
    //! \param src Pin docelowy (wejœciowy)
    //! \return Po³¹czenie pomiêdzy danymi pinami lub nullptr w przypadku braku po³¹czenia
    ConnPtr findConnection(CPinPtr src, CPinPtr dest) const;

private:
    //! Zbiór wszystkich wezlow modelu
	NODES_SET nodes;

    //! Zbiór wszystkich po³¹czeñ modelu
	CONNECTIONS_SET connections;
};

}

#endif
