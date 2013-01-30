/********************************************************************
	created:  2011/03/10
	created:  10:3:2011   17:15
	filename: CoreVisualizerWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGET_H__

#include <map>
#include <corelib/Visualizer.h>
#include <QtGui/QWidget>

class QAction;
class QLabel;
class QComboBox;
class QCheckBox;

namespace coreUI {

	class CoreWidgetAction;
	class CoreAction;

//! Widget wizualizacyjny.
class CoreVisualizerWidget : public QWidget
{
    Q_OBJECT

private:

	//! Akcja z ikoną wizualizatora - nic nie robi, zwykła labelka -> visualizerIcon
	CoreWidgetAction * iconLabelAction;
	//! Akcja odpowiedzialna za wybór danych aktualnego wizualizatora (QMenu) -> dataSelect
	CoreWidgetAction * dataSelectAction;
	//! Akcja odpowiedzialna za wybór danych aktualnego wizualizatora (QMenu) -> dataSelect
	CoreWidgetAction * activeDataSelectAction;
	//! Akcja odpowiedzialna za screenshot
	CoreAction * screenshotAction;

	//! Akcja odpowiedzialna za przełanczanie pomiędzy aktywnym obserwowaniem dataManagera w przyapdku zmian danych związanych z seriami danych
	CoreAction * liveObserveDataAction;

	//! Akcja czyszcząca wszystkie dane aktualnego wizualizatora
	QAction * dataDeselectAll;
	//! Zbiór typów danych wspieranych przez wizualizator
	core::TypeInfoSet supportedDataTypes;
	//! Zmiana aktywnej serii danych
	QComboBox * activeSerieSwitch;
    //! Bieżący wizualizator.
    core::VisualizerPtr visualizer_;
	//! Użyte indeksy dla danych bez opisu
	std::set<int> usedLocalNameIndexes;
	//! Mapa danych aktywnych - dane do serii wg typów
	std::map<core::TypeInfo, std::map<core::ObjectWrapperConstPtr, core::Visualizer::VisualizerSerie*>> activeData;
	std::map<core::Visualizer::VisualizerSerie*, int> serieLocalIdx;

public:
    //! Zerujący konstruktor.
    //! \param parent
    //! \param flags
    CoreVisualizerWidget(core::VisualizerPtr visualizer, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);        

	//CoreVisualizerWidget(const CoreVisualizerWidget & visualizer);  
            
    //! Zapewnia możliwość kasowanie widgeta wizualizatora przez jego implementację.
    ~CoreVisualizerWidget();

    //! \return Bieżący wizualizator.
    core::VisualizerPtr getVisualizer();

private:
	//! Generuje kolejny wolny lokalny identyfikator - id nie zmiania się przez cały czas dostępności danych
	//! \param startIdx Startowy identyfikator od którego zaczynamy szukać kolejnego wolnego - w szczególności będzie to on sam
	//! \return Kolejny wolny lokalny identyfikator dla danych bez nazwy
	const int nextValidLocalSerieIdx(int startIdx) const;
	//! \param data Dane dla których poszukujemy nazwy
	//! \param dataName [out] Nazwa danych
	//! \return Czy udało się pobrać nazwę dla danych
	static const bool getDataName(core::ObjectWrapperConstPtr data, std::string & dataName);
	//! \param data Dane dla których poszukujemy źródła
	//! \param dataSource [out] Nazwa źródła
	//! \return Czy udało się pobrać źródło dla danych
	static const bool getDataSource(core::ObjectWrapperConstPtr data, std::string & dataSource);
        
private slots:

	//! Usuwa wszystkie stworzone serie danych
    void removeAllSeries();  
	//! Wołane przez akcje w momencie użycia danych
	void addSerie();
	//! Wołane przez akcje w momencie usuwania danych
	void removeSerie();
    
	//! Wypełnia menu danych do wyboru
    void fillSourcesMenu();

	//! Czyści menu danych - potencjalnie zwalnia nieużywane dane
	void clearSourcesMenu();

    //! Zmiana aktywnej serii
    void serieSelected(int idx);
};

}

#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
