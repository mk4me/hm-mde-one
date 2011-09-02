#ifndef HEADER_GUARD__DFMODEL_H__
#define HEADER_GUARD__DFMODEL_H__

#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Model.h>
#include <dfmlib/DFInterface.h>
#include <map>
#include <OpenThreads/Thread>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa modelu obs�uguj�cego przep�yw danych. Model te� jest cz�ci� tego przep�ywu - zbiera informacj� o przetworzeniu danych przez w�z�y li�cie.
//! Na bazie tej informacji kontroluje moment wprowadzenia nowych danych do modelu (wyzwolenie �r�de�).
class DFModel : public Model
{
public:

    //! Typ przechowuj�cy zestaw �r�de� na potrzeby wstrzykiwania kolejnej porcji danych do data flow
	typedef std::set<DFSNPtr> SourceNodes;

    //typedef std::set<DFNPtr> Nodes;

    //typedef std::set<DFNPtr> Nodes;

//private:
//
//    class ModelRunner : public OpenThreads::Thread
//    {
//    public:
//        ModelRunner(DFModel * model);
//        ~ModelRunner();
//
//        virtual void run();
//        void finishProcessing();
//
//    private:
//        DFModel * model;
//        bool finish;
//    };
//
//    friend class ModelRunner;

public:

    //! Domy��ny konstruktor
	DFModel(void);

    //! Wirtualny destruktor
	virtual ~DFModel(void);

    //---------------------- Model interface ----------------------------
    
    //! \param node W�ze� kt�rego kompatybilno�� z modelem sprawdzamy
    //! \return Czy w�ze� jest wspierany przez model
    virtual bool isNodeSupported(const NPtr & node) const;

    //---------------------- DFModel -------------------------------------

    //! \return Zwraca zbi�r w�z��w �r�d�owych
	const SourceNodes & getSourceNodes() const;

    //! Wyzwala �r�d�a, rozpoczyna przep�yw informacji w modelu
    void run();

    //! \return Czy model jest uruchomiony (mo�e by� spauzpowany)
    bool isRunning() const;

    //! \return Czy wszystkie dane zosta�y przetworzone
    bool isFinished() const;

    //! Wstrzymuje wyzpolenie �r�de� po zako�czeniu aktualnie trwaj�cego cyklu danych
    void pause();

    //! \return Czy model jest wstrzymany (rownoznatrzne �e isRunning daje true je�li isPaused r�wnie� true)
    bool isPaused() const;

    //! Zatrzymuje model, resetuje wszystkie w�z�y i piny do ich stanu pocvz�tkowego
    void stop();

    void reset();

protected:

    //---------------- Interfejs Model --------------------

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

    //! \param node W�ze� kt�ry sta� si� w�a�nie li�ciem
    virtual void afterLeafAdd(const NPtr & node);

    //! \param node W�ze� kt�ry przestaje by� li�ciem
    virtual void beforeLeafRemove(const NPtr & node);

    //! Resetuje stany wez��w
	void resetNodeStates();

    //! Resetuje stany pin�w
	void resetPinStates();

    //! \param pin Pin kt�rego stan resetujemy
	void resetPinState(const PinPtr & pin);

    //! Powiadamia �r�d�a o konieczno�ci zasilenia model w nowe dane
	void notifySources();
	
    //! \return Prawda je�li �r�d�a maj� wi�cej danych do zasilenia modelu
	bool sourcesHaveMoreData() const;

private:

    //! Informuje �e li�� przetworzy� dane
	void leafHasProcessedData();

private:

    //! Zbi�r w�z��w �r�d�owych
	SourceNodes sourceNodes;

    //! Czy model jest uruchomiony
    bool running;

    //! Czy wszystkie dane przetworzone
    bool finished;

    //! Czy model jest spauzowany
    bool paused;

    bool stopPending;

    bool pausePending;

    //! Licznik ilo�ci w�z��w li�ci kt�re przetworzy�y dane
	unsigned int finishedLeafes;

    //! mutex do kontroli w�tku notyfikujacego �r�d�a
    //OpenThreads::Mutex pauseMutex;

    //! mutex do synchronizacji aktualizacji ilo�ci li�ci kt�re przetworzy�y dane
    //OpenThreads::Mutex leavesMutex;

    //! mutex do synchronizacji operacji uruchamiania/zatrzymywania/pauzowania dataflow
    mutable OpenThreads::ReentrantMutex runningMutex;

    //! w�tek obs�uguj�cy wyzwalanie �r�de�
    //boost::shared_ptr<ModelRunner> modelRunner;
};

}

#endif