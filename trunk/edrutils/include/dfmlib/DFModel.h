#ifndef HEADER_GUARD__DFMODEL_H__
#define HEADER_GUARD__DFMODEL_H__

#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Model.h>
#include <dfmlib/DFInterface.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa modelu obs�uguj�cego przep�yw danych. Model te� jest cz�ci� tego przep�ywu - zbiera informacj� o przetworzeniu danych przez w�z�y li�cie.
//! Na bazie tej informacji kontroluje moment wprowadzenia nowych danych do modelu (wyzwolenie �r�de�).
class DFModel : public Model, public DFInterface
{
public:

    //! Typ przechowuj�cy lokalne niezgodno�ci modelu logicznego z wytyczonymi zasadami jego tworzenia
	typedef std::map<NPtr, Node::PINS_SET > REQUIRING_CONNECTION;

    //! Typ przechowuj�cy zestaw �r�de� na potrzeby wstrzykiwania kolejnej porcji danych do data flow
	typedef std::set<DFSNPtr> SOURCE_NODES_SET;

public:

    //! Domy��ny konstruktor
	DFModel(void);

    //! Wirtualny destruktor
	virtual ~DFModel(void);

    //---------------------- Model interface ----------------------------

    //! \param node W�ze� do dodania
	virtual bool addNode(NPtr node);

    //! \param node W�ze� do usuniecia
	virtual bool removeNode(NPtr node);

    //! Usuwa wszystkie wezly z modelu
	virtual void clearNodes();

    //! \param src Pin �r�d�owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
    //! \return Czy mo�na polaczy� piny razem
	virtual bool canConnect(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin �r�d�owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
    //! \return Po��czenie pomi�dzy pinami
	virtual ConnPtr connect(PinPtr src, PinPtr dest);

    //! \param src Pin �r�d�owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
	//virtual bool removeConnection(PinPtr src, PinPtr dest);

    //! \param connection Po��czenie do usuni�cia
	virtual bool removeConnection(ConnPtr connection);

    //! Czy�ci wszystkie po��czenia mi�dzy w�z�ami
	virtual void clearConnections();

    //---------------------- DFModel -------------------------------------

    //! \return Zwraca zbi�r w�z��w �r�d�owych
	const SOURCE_NODES_SET & getSourceNodes() const;

    //! \return Zwraca zbi�r w�z��w li�ci (nie maj�cych pod��czonych pin�w wyj�ciowych)
	const NODES_SET & getLeafNodes() const;

    //! \return Zwraca w�z�y wymagaj�ce interwencji by model by� poprawny
	const REQUIRING_CONNECTION & DFModel::getRequiringConnections() const;

protected:

    //---------------- Interfejs DFInterface --------------------

    virtual void onEnableChange();

    virtual void process();

    //! \return Czy model jest poprawnie zbudowany
	virtual bool validateModel() const;

    //! Resetuje stany wez��w
	void resetNodeStates();

    //! Resetuje stany pin�w
	void resetPinStates();

    //! \param pin Pin kt�rego stan resetujemy
	void resetPinState(PinPtr pin);

    //! Powiadamia �r�d�a o konieczno�ci zasilenia model w nowe dane
	void notifySources();

    //! \param src Pin �r�d�owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
    //! \return Po��czenie pomi�dzy pinami
	virtual ConnPtr quickConnect(PinPtr src, PinPtr dest);
    	
    //! \param node W�ze� do sprawdzenia, czy wszystko ok z jego po��czeniami i stanem
	void updateRequiredConnections(NPtr node);

    //TODO
    //zmiana na obslug� wyj�tk�w i wielow�tkowo�ci
    //! \return Prawda je�li mo�na wykona� operacj� modyfikacji modelu
	bool validateOperation() const;

    //! \param node W�ze� do sprawdzenia czy jest pochodn� DFNode
    //! \return Czy mo�na usun�� w�ze�
	static bool validateNodeType(NPtr node);

    //! \param connection Po��czenie do sprawdzenia pod k�tem jego usuni�cia
    //! \return Prawda jesli po��czneie mo�e zostac usuni�te
	static bool canDisconnect(ConnPtr connection);

    //! \param node W�ze� do sprawdzenia pod k�tem jego usuni�cia
    //! \return Prawda jesli w�ze� mo�e zostac usuni�ty
	static bool canRemoveNode(NPtr node);

    //! \param connection Po��czenie do usuni�cia
    //! \return czy usuni�to po��czenie
	bool quickRemoveConnection(ConnPtr connection);
	
    //! \return Prawda je�li �r�d�a maj� wi�cej danych do zasilenia modelu
	bool sourcesHaveMoreData() const;

    //! \param pin Pin analizowany pod k�tem poprawno�ci po��czenia modelu
	void updateRequireConnections(PinPtr pin);

private:

    //! Prywatny typ opisuj�cy aktualn� pozycj� podczas iterowania po po��czeniach w�z��w przy przechodzeniu przez model
	typedef std::pair<Node::PINS_SET::const_iterator,
		CONNECTIONS_SET::const_iterator> OUTPUT_POSITION;

    //! Prywatna klasa pinu dodawanego do w�z��w li�ci w celu notyfikacji modelu o dotarciu danych do li�ci
	class FakePin : public virtual DFPin{

	public:
        //! \param model DFModel kt�ry FakePin poinformuje o przetworzeniu danych przez w�ze� li��
		FakePin(WDFMPtr model);

        //! \param pin Pin do sprawdzenia kompatybilno�ci z FakePin
		bool isCompatible(PinPtr pin) const;
	protected:

        //! \return
		bool onUpdate();
	private:

        //! Model informowany o zako�czeniu przetwarzania danych przez w�ze� li��
		WDFMPtr model;
	};

	friend class FakePin;

private:

    //! \param node W�ze� li�� do zapami�tania
	void addLeaf(NPtr node);

    //! \param node W�ze� li�� przestaj�cy by� li�ciem
	void removeLeaf(NPtr node);

    //! Informuje �e li�� przetworzy� dane
	void leafHasProcessedData();

private:

    //! Zbi�r w�z��w �r�d�owych
	SOURCE_NODES_SET sourceNodes;

    //! Zbi�r w�z��w li�ci
	NODES_SET leafNodes;

    //! Mapa w�z��w li�ci i ich FakePin�w
	//Node -> FakePin
	std::map<NPtr, PinPtr> leafFakePins;

    //! Mapa w�z��w wymagaj�cych interwencji aby model by� poprawnie zbudowany
	REQUIRING_CONNECTION pinsRequiringConnections;

    //! Oczekiwanie na przerwanie procesu przetwarzania danych
	bool pendingDisable;

    //! Licznik ilo�ci w�z��w li�ci kt�re przetworzy�y dane
	unsigned int finishedLeafes;
};

}

#endif