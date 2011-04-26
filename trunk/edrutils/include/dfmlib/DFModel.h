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

//! Klasa modelu obs³uguj¹cego przep³yw danych. Model te¿ jest czêœci¹ tego przep³ywu - zbiera informacjê o przetworzeniu danych przez wêz³y liœcie.
//! Na bazie tej informacji kontroluje moment wprowadzenia nowych danych do modelu (wyzwolenie Ÿróde³).
class DFModel : public Model, public DFInterface
{
public:

    //! Typ przechowuj¹cy lokalne niezgodnoœci modelu logicznego z wytyczonymi zasadami jego tworzenia
	typedef std::map<NPtr, Node::PINS_SET > REQUIRING_CONNECTION;

    //! Typ przechowuj¹cy zestaw Ÿróde³ na potrzeby wstrzykiwania kolejnej porcji danych do data flow
	typedef std::set<DFSNPtr> SOURCE_NODES_SET;

public:

    //! Domyœ³ny konstruktor
	DFModel(void);

    //! Wirtualny destruktor
	virtual ~DFModel(void);

    //---------------------- Model interface ----------------------------

    //! \param node Wêze³ do dodania
	virtual bool addNode(NPtr node);

    //! \param node Wêze³ do usuniecia
	virtual bool removeNode(NPtr node);

    //! Usuwa wszystkie wezly z modelu
	virtual void clearNodes();

    //! \param src Pin Ÿród³owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
    //! \return Czy mo¿na polaczyæ piny razem
	virtual bool canConnect(CPinPtr src, CPinPtr dest) const;

    //! \param src Pin Ÿród³owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
    //! \return Po³¹czenie pomiêdzy pinami
	virtual ConnPtr connect(PinPtr src, PinPtr dest);

    //! \param src Pin Ÿród³owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
	//virtual bool removeConnection(PinPtr src, PinPtr dest);

    //! \param connection Po³¹czenie do usuniêcia
	virtual bool removeConnection(ConnPtr connection);

    //! Czyœci wszystkie po³¹czenia miêdzy wêz³ami
	virtual void clearConnections();

    //---------------------- DFModel -------------------------------------

    //! \return Zwraca zbiór wêz³ów Ÿród³owych
	const SOURCE_NODES_SET & getSourceNodes() const;

    //! \return Zwraca zbiór wêz³ów liœci (nie maj¹cych pod³¹czonych pinów wyjœciowych)
	const NODES_SET & getLeafNodes() const;

    //! \return Zwraca wêz³y wymagaj¹ce interwencji by model by³ poprawny
	const REQUIRING_CONNECTION & DFModel::getRequiringConnections() const;

protected:

    //---------------- Interfejs DFInterface --------------------

    virtual void onEnableChange();

    virtual void process();

    //! \return Czy model jest poprawnie zbudowany
	virtual bool validateModel() const;

    //! Resetuje stany wez³ów
	void resetNodeStates();

    //! Resetuje stany pinów
	void resetPinStates();

    //! \param pin Pin którego stan resetujemy
	void resetPinState(PinPtr pin);

    //! Powiadamia Ÿród³a o koniecznoœci zasilenia model w nowe dane
	void notifySources();

    //! \param src Pin Ÿród³owy (wyjsciowy)
    //! \param dest Pin docelowy (wejsciowy)
    //! \return Po³¹czenie pomiêdzy pinami
	virtual ConnPtr quickConnect(PinPtr src, PinPtr dest);
    	
    //! \param node Wêze³ do sprawdzenia, czy wszystko ok z jego po³¹czeniami i stanem
	void updateRequiredConnections(NPtr node);

    //TODO
    //zmiana na obslugê wyj¹tków i wielow¹tkowoœci
    //! \return Prawda jeœli mo¿na wykonaæ operacjê modyfikacji modelu
	bool validateOperation() const;

    //! \param node Wêze³ do sprawdzenia czy jest pochodn¹ DFNode
    //! \return Czy mo¿na usun¹æ wêze³
	static bool validateNodeType(NPtr node);

    //! \param connection Po³¹czenie do sprawdzenia pod k¹tem jego usuniêcia
    //! \return Prawda jesli po³¹czneie mo¿e zostac usuniête
	static bool canDisconnect(ConnPtr connection);

    //! \param node Wêze³ do sprawdzenia pod k¹tem jego usuniêcia
    //! \return Prawda jesli wêze³ mo¿e zostac usuniêty
	static bool canRemoveNode(NPtr node);

    //! \param connection Po³¹czenie do usuniêcia
    //! \return czy usuniêto po³¹czenie
	bool quickRemoveConnection(ConnPtr connection);
	
    //! \return Prawda jeœli Ÿród³a maj¹ wiêcej danych do zasilenia modelu
	bool sourcesHaveMoreData() const;

    //! \param pin Pin analizowany pod k¹tem poprawnoœci po³¹czenia modelu
	void updateRequireConnections(PinPtr pin);

private:

    //! Prywatny typ opisuj¹cy aktualn¹ pozycjê podczas iterowania po po³¹czeniach wêz³ów przy przechodzeniu przez model
	typedef std::pair<Node::PINS_SET::const_iterator,
		CONNECTIONS_SET::const_iterator> OUTPUT_POSITION;

    //! Prywatna klasa pinu dodawanego do wêz³ów liœci w celu notyfikacji modelu o dotarciu danych do liœci
	class FakePin : public virtual DFPin{

	public:
        //! \param model DFModel który FakePin poinformuje o przetworzeniu danych przez wêze³ liœæ
		FakePin(WDFMPtr model);

        //! \param pin Pin do sprawdzenia kompatybilnoœci z FakePin
		bool isCompatible(PinPtr pin) const;
	protected:

        //! \return
		bool onUpdate();
	private:

        //! Model informowany o zakoñczeniu przetwarzania danych przez wêze³ liœæ
		WDFMPtr model;
	};

	friend class FakePin;

private:

    //! \param node Wêze³ liœæ do zapamiêtania
	void addLeaf(NPtr node);

    //! \param node Wêze³ liœæ przestaj¹cy byæ liœciem
	void removeLeaf(NPtr node);

    //! Informuje ¿e liœæ przetworzy³ dane
	void leafHasProcessedData();

private:

    //! Zbiór wêz³ów Ÿród³owych
	SOURCE_NODES_SET sourceNodes;

    //! Zbiór wêz³ów liœci
	NODES_SET leafNodes;

    //! Mapa wêz³ów liœci i ich FakePinów
	//Node -> FakePin
	std::map<NPtr, PinPtr> leafFakePins;

    //! Mapa wêz³ów wymagaj¹cych interwencji aby model by³ poprawnie zbudowany
	REQUIRING_CONNECTION pinsRequiringConnections;

    //! Oczekiwanie na przerwanie procesu przetwarzania danych
	bool pendingDisable;

    //! Licznik iloœci wêz³ów liœci które przetworzy³y dane
	unsigned int finishedLeafes;
};

}

#endif