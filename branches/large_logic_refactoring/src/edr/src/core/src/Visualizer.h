/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   15:08
    filename: Visualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZER_H__
#define HEADER_GUARD_CORE__VISUALIZER_H__

#include <core/SmartPtr.h>
#include <core/IVisualizer.h>
#include <QtCore/QObject>

namespace core {

//! Wizualizator. Jego zadaniem jest stworzyć widget (gdzie on będzie osadzony - nie jego sprawa),
//! zarządzać seriami danych wizualizatora, informować o dodaniu/usunięciu serii danych, automatycznie
	//! aktualizować serie danych w momencie edycji związanych z nimi danych w DM
class Visualizer : public QObject
{
	Q_OBJECT;

private:
	//! Forward declaration obiektu pomocniczego dla vizualizatora, przykrywa część implementacji
	class VisualizerHelper;

public:

	//! Seria vizualizatora
	class VisualizerSerie
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

	//! Agregat aktualnie istniejących serii danych
    typedef std::set<VisualizerSerie*> DataSeries;

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

private:
	//! Inicjalizacja wizualizatora przy tworzeniu
	void init();
	//! Notyfikacja o zmianie
	//! \param serie Seria która ulega zmianie
	//! \param modyfication Typ modyfikacji serii
	void notifyChange(VisualizerSerie * serie, SerieModyfication modyfication);

protected:
    //! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
    Visualizer( const plugin::IVisualizer* proto );
    //! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta.
    Visualizer( const Visualizer& visualizer );

public:
    //! Destruktor 
    virtual ~Visualizer();

signals:

	void screenshotTaken(QPixmap & screenshot);

public slots:

	void onScreenshotTrigger();

public:
    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Może być nullptr, jeżeli jeszcze nie jest stworzony.
    QWidget* getWidget();
	//! \return Nazwa wizualizatora
	const QString getName() const;
    //! \return Ikona wizualizatora
    const QIcon getIcon() const;
	//! \return Maksymalna ilość serii jaką wizualizator może obsłuzyć
    const int getMaxSeries() const;
	//! \param data Dane na bazie których ma powstać seria, muszą być wspierane przez wizualizator,
	//! w przeciwnym wypadku seria nie zostanie stworozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
	//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
    VisualizerSerie * createSerie(const ObjectWrapperConstPtr & data);
	//! \param serie Seria na bazie której ma powstać nowa seria, musi pochodzić od tego wizualizatora,
	//! w przeciwnym wypadku seria nie zostanie stworzozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
	//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
    VisualizerSerie * createSerie(VisualizerSerie * serie);
	//! \param serie Seria do zniszczenia, musi pochodzić z tego wizualizatora
    void destroySerie(VisualizerSerie * serie);
	//! \return Zbiór wspieranych typów przez wizualizator (od nich można też brać pochodne, któe je wspierają -> iść w górę ścieżki typów)
	const TypeInfoSet & getSupportedTypes() const;
	//! Usuwa wszystkie serie
    void destroyAllSeries();

	//! \param type Typ dla którego chcemy pobrać dane związane z wizualizatorem
	//! \param objects [out] Agregat dla danych zadanego typu związanych z wizualizatorem
	virtual void getData(const TypeInfo & type, ConstObjectsList & objects) const = 0;

	//! \param observer Obserwator do dodania
	void addObserver(IVisualizerObserver * observer);
	//! \param observer Obserwator do usunięcia
	void removeObserver(IVisualizerObserver * observer);
};

//! Wizualizator będący przeglądarką wszystkich danych w DM
class BrowserVisualizer : public Visualizer
{
public:
	//! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
	BrowserVisualizer( const plugin::IVisualizer* proto );
	//! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta.
	BrowserVisualizer( const BrowserVisualizer& visualizer );

	virtual void getData(const TypeInfo & type, ConstObjectsList & objects) const;
};

//! Wizualizator działający w oparciu o przypisane dane
class DataManagebleVisualizer : public Visualizer
{
public:
	//! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
	DataManagebleVisualizer(const plugin::IVisualizer* proto );
	//! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta.
	DataManagebleVisualizer( const DataManagebleVisualizer& visualizer );

	virtual void getData(const TypeInfo & type, ConstObjectsList & objects) const;
	void addData(ObjectWrapperCollection & data);
	void removeData(const TypeInfo & type);
	void removeData(const TypeInfo & type, ObjectWrapperConstPtr data);

private:
	std::map<TypeInfo, ConstObjects> data_;
};

typedef core::shared_ptr<Visualizer> VisualizerPtr;
typedef core::shared_ptr<const Visualizer> VisualizerConstPtr;
typedef core::weak_ptr<Visualizer> VisualizerWeakPtr;
typedef core::weak_ptr<const Visualizer> VisualizerConstWeakPtr;

}

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__
