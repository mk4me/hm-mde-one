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

//! Klasa opisujaca model logiczny data flow. Pozwala na jego edycj� - dodawanie, usuwanie w�z��w oraz ��czenie/roz�anczanie pin�w wg �ci�le okre�lonych zasad.
class Model : public boost::enable_shared_from_this<Model>
{
public:
    //! Typ opisuj�cy p�tle/cykle w po�aczeniach
	typedef std::vector<ConnPtr> CYCLE;

    //! Typreprezentuj�cy zbi�r w�z��w
	typedef std::set<NPtr> NODES_SET;

    //! Typ opisuj�cy zbi�r po��cze�
	typedef Pin::CONNECTIONS_SET CONNECTIONS_SET;

private:

    //! Struktura pomocnicza przy analizie p�tli (cykl�w) w modelu
    typedef struct MY_PATH_ENTRY{
        //! Aktualny w�ze�
        NPtr node;
        //! Aktualny pin
        Node::PINS_SET::const_iterator pinIT;

        //! Aktualne po��czenie
        CONNECTIONS_SET::const_iterator connIT;

        //! Operator por�wnania
        bool operator==(const MY_PATH_ENTRY & pe) const;
    }PATH_ENTRY;

public:

    //! Konstruktor zeruj�cy
	Model(void);

    //! Wirtualny destruktor
	virtual ~Model(void);

    //! \param node W�ze� do dodania
    virtual void addNode(NPtr node);

    //! \param node W�ze� do usuni�cia
    virtual void removeNode(NPtr node);

    //! Usuwa wszystkie w�z�y i po��czenia z modelu
	virtual void clearNodes();

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Czy mo�na po�aczy� piny ze wzgl�du na obowi�zuj�ce regu�y
	virtual bool canConnect(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Czy po��czenie tych pin�w wprowadzi cykl (p�tle) w modelu
	bool createCycle(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Opis cyklu (�ciezki) jaki powstanie przez po��czenie tych dw�ch pin�w
	virtual CYCLE getCycle(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Po��czenie utworzone pomi�dzy zadanymi pinami
	virtual ConnPtr connect(PinPtr src, PinPtr dest);

    //! \param connection Po��czenie do usuni�cia
    virtual void removeConnection(ConnPtr connection);

    //! Usuwa wszystkie po��czenia w modelu
	virtual void clearConnections();

    //! \return Wszystkie w�z�y modelu
	const NODES_SET & getNodes() const;

    //! \return Wszystkie po��czenia w modelu
	const CONNECTIONS_SET & getConnections() const;
	
 protected:

     static void unregisterPin(PinPtr pin);
    
     //! \param Weze� do rozlaczenia - wszystke jego po�aczenia zostaj� usuniete
     virtual void disconnectNode(NPtr node);
    
    //! \param connection Po�aczenie do usuniecia z modelu
     virtual void quickRemoveConnection(ConnPtr connection);

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Po�aczenie miedzy pinami
	virtual ConnPtr quickConnect(PinPtr src, PinPtr dest);

    //! \param nodes Lista w�z��w uznana za sprawdzone - tutaj mog� si� podpina� klasy pochodne z szersz� wiedz� na temat w�z��w
    //! W ten spos�b mog� przyspieszy� wykrywanie cykli
    virtual void initCheckedNodes(NODES_SET & nodes) const;

private:
    //! \param node Weze� dla kt�rego tworzymy strukture opisujaca kolejnosc analizowania polaczen dla sprawdzania cykli
    //! \return Aktualny stan analizowanych polaczen
    static PATH_ENTRY getFirstNodeOutputConnection(NPtr node);

    //! \param pathElement Element opisujacy kolejnosc przegladania po��cze� w w�le - jej aktualny stan
    //! \return Kolejny stan analizowanych polaczen
    static PATH_ENTRY getNextNodeOutputConnection(const PATH_ENTRY & pathElement);

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    //! \return Po��czenie pomi�dzy danymi pinami lub nullptr w przypadku braku po��czenia
    ConnPtr findConnection(CPinPtr src, CPinPtr dest) const;

private:
    //! Zbi�r wszystkich wezlow modelu
	NODES_SET nodes;

    //! Zbi�r wszystkich po��cze� modelu
	CONNECTIONS_SET connections;
};

}

#endif
