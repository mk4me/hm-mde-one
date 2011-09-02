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

//! Klasa modelu obs³uguj¹cego przep³yw danych. Model te¿ jest czêœci¹ tego przep³ywu - zbiera informacjê o przetworzeniu danych przez wêz³y liœcie.
//! Na bazie tej informacji kontroluje moment wprowadzenia nowych danych do modelu (wyzwolenie Ÿróde³).
class DFModel : public Model
{
public:

    //! Typ przechowuj¹cy zestaw Ÿróde³ na potrzeby wstrzykiwania kolejnej porcji danych do data flow
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

    //! Domyœ³ny konstruktor
	DFModel(void);

    //! Wirtualny destruktor
	virtual ~DFModel(void);

    //---------------------- Model interface ----------------------------
    
    //! \param node Wêze³ którego kompatybilnoœæ z modelem sprawdzamy
    //! \return Czy wêze³ jest wspierany przez model
    virtual bool isNodeSupported(const NPtr & node) const;

    //---------------------- DFModel -------------------------------------

    //! \return Zwraca zbiór wêz³ów Ÿród³owych
	const SourceNodes & getSourceNodes() const;

    //! Wyzwala Ÿród³a, rozpoczyna przep³yw informacji w modelu
    void run();

    //! \return Czy model jest uruchomiony (mo¿e byæ spauzpowany)
    bool isRunning() const;

    //! \return Czy wszystkie dane zosta³y przetworzone
    bool isFinished() const;

    //! Wstrzymuje wyzpolenie Ÿróde³ po zakoñczeniu aktualnie trwaj¹cego cyklu danych
    void pause();

    //! \return Czy model jest wstrzymany (rownoznatrzne ¿e isRunning daje true jeœli isPaused równie¿ true)
    bool isPaused() const;

    //! Zatrzymuje model, resetuje wszystkie wêz³y i piny do ich stanu pocvz¹tkowego
    void stop();

    void reset();

protected:

    //---------------- Interfejs Model --------------------

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

    //! \param node Wêze³ który sta³ siê w³aœnie liœciem
    virtual void afterLeafAdd(const NPtr & node);

    //! \param node Wêze³ który przestaje byæ liœciem
    virtual void beforeLeafRemove(const NPtr & node);

    //! Resetuje stany wez³ów
	void resetNodeStates();

    //! Resetuje stany pinów
	void resetPinStates();

    //! \param pin Pin którego stan resetujemy
	void resetPinState(const PinPtr & pin);

    //! Powiadamia Ÿród³a o koniecznoœci zasilenia model w nowe dane
	void notifySources();
	
    //! \return Prawda jeœli Ÿród³a maj¹ wiêcej danych do zasilenia modelu
	bool sourcesHaveMoreData() const;

private:

    //! Informuje ¿e liœæ przetworzy³ dane
	void leafHasProcessedData();

private:

    //! Zbiór wêz³ów Ÿród³owych
	SourceNodes sourceNodes;

    //! Czy model jest uruchomiony
    bool running;

    //! Czy wszystkie dane przetworzone
    bool finished;

    //! Czy model jest spauzowany
    bool paused;

    bool stopPending;

    bool pausePending;

    //! Licznik iloœci wêz³ów liœci które przetworzy³y dane
	unsigned int finishedLeafes;

    //! mutex do kontroli w¹tku notyfikujacego Ÿród³a
    //OpenThreads::Mutex pauseMutex;

    //! mutex do synchronizacji aktualizacji iloœci liœci które przetworzy³y dane
    //OpenThreads::Mutex leavesMutex;

    //! mutex do synchronizacji operacji uruchamiania/zatrzymywania/pauzowania dataflow
    mutable OpenThreads::ReentrantMutex runningMutex;

    //! w¹tek obs³uguj¹cy wyzwalanie Ÿróde³
    //boost::shared_ptr<ModelRunner> modelRunner;
};

}

#endif