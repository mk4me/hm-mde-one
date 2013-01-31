/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   15:08
    filename: Visualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZER_H__
#define HEADER_GUARD_CORE__VISUALIZER_H__

#include <corelib/Export.h>
#include <corelib/SmartPtr.h>
#include <corelib/ObjectWrapperCollection.h>
#include <corelib/IVisualizer.h>
#include <QtCore/QObject>
#include <QtGui/QIcon>

namespace core {

	class IDataManagerReader;
	class IVisualizerManager;

//! Wizualizator. Jego zadaniem jest stworzyć widget (gdzie on będzie osadzony - nie jego sprawa),
//! zarządzać seriami danych wizualizatora, informować o dodaniu/usunięciu serii danych, automatycznie
	//! aktualizować serie danych w momencie edycji związanych z nimi danych w DM
class CORELIB_EXPORT Visualizer : public QObject
{
	Q_OBJECT;

private:
	//! Forward declaration obiektu pomocniczego dla vizualizatora, przykrywa część implementacji
	class VisualizerHelper;
	friend class VisualizerHelper;

public:

	//! Seria vizualizatora
	class CORELIB_EXPORT VisualizerSerie
	{
		friend class Visualizer;
	private:
		//! Prywatny konstruktor - tylko wizualizator może tworzyć takie obiekty
		//! \param visualizer Wizualizator który stworzył ta serię
		//! \param serieBase Faktyczna seria zaimplementowana przez opakowany wizualizator
		//! \param timeSerieFeatures Możliwości serii pod kątem zarządzania czasem - nie muszą być implementowane -> wtedy nullptr
		VisualizerSerie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase, plugin::IVisualizer::ITimeSerieFeatures * timeSerieFeatures);
		//! Prywatny destruktor - tylko wizualizator może niszczyć serie danych
		~VisualizerSerie();

	public:
		//! \return Podstawowy interfejs serii danych
		plugin::IVisualizer::ISerie * serie() const;
		//! \return Czasowy interfejs serii danych - nie musi być implementowany przez serię, wtedy nullptr
		plugin::IVisualizer::ITimeSerieFeatures * timeSerieFeatures() const;

	private:
		//! Wizualizator który utworzył tę serię danych
		Visualizer * visualizer_;
		//! Podstawowy interfejs serii danych
		plugin::IVisualizer::ISerie * serie_;
		//! Czasowy interfejs serii danych - nie musi być implementowany przez serię, wtedy nullptr
		plugin::IVisualizer::ITimeSerieFeatures * timeSerieFeatures_;
	};

	//! Interfejs źródła danych wizualizatora
	class VisualizerDataSource
	{
	public:
		//! \param type Typdanych jaki nas interesuje
		//! \param objects [out] Lista obiektów zadanego typu
		//! \param exact Czy typ ma się zgadzać czy może być też typ pochodny
		virtual void getData(const TypeInfo & type, ConstObjectsList & objects, bool exact = false) const;
	};

	//! Smart pointer do źródła
	typedef shared_ptr<VisualizerDataSource> VisualizerDataSourcePtr;
	//! Agregat źródeł danych
	typedef std::list<VisualizerDataSourcePtr> DataSources;

	//! Agregat aktualnie istniejących serii danych
    typedef std::list<VisualizerSerie*> DataSeries;

	//! Opis zmian w seriach danych
	enum SerieModyfication {
		ADD_SERIE,		//! Dodanie serii danych, notyfikacja wywoływana po dodaniu serii, przed return
		REMOVE_SERIE	//! Usunięcie serii danych, notyfikacja wywoływana przed usunięciem serii
	};

	//! Interfejs obiektu obserwującego zmiany w seriach danych wizualizatora
	class IVisualizerObserver
	{
	public:
		//! \param serie Seria która ulega modyfikacji
		//! \param modyfication Typ modyfikacji na serii danych
		virtual void update(VisualizerSerie * serie, SerieModyfication modyfication ) = 0;
	};

private:
	//! Obiekt obserwujący dane w MemoryDM - tylko update nas interesuje aby odświeżać serię
	//! Reszta będzie robiona w widgetach na odpowiednie akcje (czy dane są jeszcze w DM)
	shared_ptr<VisualizerHelper> visualizerHelper_;
	//! Faktyczny wizualizator dla danych
	scoped_ptr<plugin::IVisualizer> visualizer_;

    //! Faktyczny widget.
    QWidget* widget;
	//! Ikona widgeta
	QIcon icon;
	//! Wspierane typy danych przez wizualizator
	TypeInfoSet supportedTypes;
    //! Serie danych utrworzone przez użytkownika
    DataSeries dataSeries;
	//! Lista obserwujących
	std::list<IVisualizerObserver*> observers_;

	//! DataManagerReader
	IDataManagerReader * dmr;
	//! VisualizerManager
	IVisualizerManager * visManager;
	//! Aktywna seria
	VisualizerSerie * activeSerie;

private:
	//! Inicjalizacja wizualizatora przy tworzeniu
	void init();
	//! Notyfikacja o zmianie
	//! \param serie Seria która ulega zmianie
	//! \param modyfication Typ modyfikacji serii
	void notifyChange(VisualizerSerie * serie, SerieModyfication modyfication);

signals:
	//! \param screenshot Screen z wizualizatora
	void screenshotTaken(QPixmap & screenshot);

public slots:

	void onScreenshotTrigger();

public:

	//! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
	Visualizer( const plugin::IVisualizer* proto, IDataManagerReader * dmr, IVisualizerManager * visManager );
	//! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta, źródeł danych, obserwatorów
	Visualizer( const Visualizer& visualizer );

	//! Destruktor 
	~Visualizer();

	//! \return Unikalny identyfikator obiektu
	core::UniqueID getID() const;
	//! \return nazwa wizualizatora
	const std::string getName() const;
	//! \return Opis wizualizatora
	const std::string  getDescription() const;
	//! \return Klon wizualizatora poza konfiguracją sesji
	Visualizer * create() const;

	//! \param Czy vizualizator powinien sam automatycznie obserwować zmiany po stronie dataManagera dla danych związanych z seriami danych
	void setLiveObserveActive(bool active);
	//! \return Czy vizualizator powinien sam automatycznie obserwować zmiany po stronie dataManagera dla danych związanych z seriami danych
	const bool isLiveObserveActive() const;

    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Może być nullptr, jeżeli jeszcze nie jest stworzony.
    QWidget* getWidget();
    //! \return Ikona wizualizatora
    const QIcon getIcon() const;
	//! \return Maksymalna ilość serii jaką wizualizator może obsłuzyć
    const int getMaxSeries() const;
	//! \param data Dane na bazie których ma powstać seria, muszą być wspierane przez wizualizator,
	//! w przeciwnym wypadku seria nie zostanie stworozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
	//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
    VisualizerSerie * createSerie(const TypeInfo & requestedType, const ObjectWrapperConstPtr & data);
	//! \param serie Seria na bazie której ma powstać nowa seria, musi pochodzić od tego wizualizatora,
	//! w przeciwnym wypadku seria nie zostanie stworzozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
	//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
    VisualizerSerie * createSerie(VisualizerSerie * serie);
	//! \param serie Seria do zniszczenia, musi pochodzić z tego wizualizatora
    void destroySerie(VisualizerSerie * serie);
	//! \return Ilośc serii w wizualizatorze
	const int getNumSeries() const;
	//! \param idx Indeks serii
	//! \return Seria dla zadanego indeksu
	VisualizerSerie * getSerie(int idx);
	//! \param serie Seria danych wizualizatora
	//! \return Aktualny indeks serii danych
	const int serieIdx(VisualizerSerie * serie) const;

	//! Ustawia daną serię aktywną
	void setActiveSerie(VisualizerSerie * serie);
	//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
	const VisualizerSerie * getActiveSerie() const;
	//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
	VisualizerSerie * getActiveSerie();


	//! \return Zbiór wspieranych typów przez wizualizator (od nich można też brać pochodne, któe je wspierają -> iść w górę ścieżki typów)
	void getSupportedTypes(TypeInfoSet & supportedTypes) const;
	//! Usuwa wszystkie serie
    void destroyAllSeries();

	//! \param observer Obserwator do dodania
	void addObserver(IVisualizerObserver * observer);
	//! \param observer Obserwator do usunięcia
	void removeObserver(IVisualizerObserver * observer);

	//! \param datSource Źródło danych wizualizatora do dodania
	void addDataSource(VisualizerDataSourcePtr dataSource);
	//! \param datSource Źródło danych wizualizatora do usunięcia
	void removeDataSource(VisualizerDataSourcePtr dataSource);
	//! \return ilość źródeł
	const int getNumDataSources() const;
	//! \param idx Indeks źródła które chcemy pobrać
	VisualizerDataSourcePtr getDataSource(int idx);

	//! \param type Typ danych jaki chcemy pobrać
	//! \param objects [out] Lista obiektów z danymi zadanego typu
	//! \param exact Czy interesują nas tylko typy zgodne czy mogą być też pochodne
	void getData(const TypeInfo & type, ConstObjectsList & objects, bool exact);
};

typedef core::shared_ptr<Visualizer> VisualizerPtr;
typedef core::shared_ptr<const Visualizer> VisualizerConstPtr;
typedef core::weak_ptr<Visualizer> VisualizerWeakPtr;
typedef core::weak_ptr<const Visualizer> VisualizerConstWeakPtr;

}

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__
