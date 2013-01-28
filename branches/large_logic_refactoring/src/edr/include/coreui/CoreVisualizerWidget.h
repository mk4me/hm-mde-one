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
#include <coreui/CoreAction.h>

class QLabel;
class QComboBox;

namespace coreUI {

	class CoreWidgetAction;

//! Widget wizualizacyjny.
class CoreVisualizerWidget : public QWidget
{
    Q_OBJECT

private:

	//! Akcja z ikoną wizualizatora - nic nie robi, zwykła labelka -> visualizerIcon
	CoreWidgetAction * iconLabelAction;
	//! Akcja zmieniająca aktualny wizualizator (QComboBox) -> visualizerSwitch
	CoreWidgetAction * visualizerSwitchAction;
	//! Akcja odpowiedzialna za wybór danych aktualnego wizualizatora (QMenu) -> dataSelect
	CoreWidgetAction * dataSelectAction;
	//! Akcja odpowiedzialna za wybór danych aktualnego wizualizatora (QMenu) -> dataSelect
	CoreWidgetAction * activeDataSelectAction;
	//! Akcja czyszcząca wszystkie dane aktualnego wizualizatora
	QAction * dataDeselectAll;

	QComboBox * visualizerSwitch;

    //! Bieżący wizualizator.
    core::VisualizerPtr currentVisualizer_;
	//! Lista wszystkich zarejestrowanych wizualizatorów
	std::list<core::VisualizerPtr> visualizers_;

public:
    //! Zerujący konstruktor.
    //! \param parent
    //! \param flags
    CoreVisualizerWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);        

	CoreVisualizerWidget(const CoreVisualizerWidget & visualizer);  
            
    //! Zapewnia możliwość kasowanie widgeta wizualizatora przez jego implementację.
    ~CoreVisualizerWidget();
        
public:

    //! Dodaje wizualizator do listy obsługiwanych.
    void addVisualizer(core::VisualizerPtr visualizer, const QString & name = QString());

	//! Dodaje wizualizator do listy obsługiwanych.
	void removeVisualizer(core::VisualizerPtr visualizer);

    //! \return Liczba zarejestrowanych wizualizatorów.
    int getNumVisualizers() const;

	core::VisualizerPtr getVisualizer(int idx);

	core::VisualizerConstPtr getVisualizer(int idx) const;

    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan początkowy.
    void clearCurrentVisualizer();

    //! \return Bieżący wizualizator.
    core::VisualizerPtr getCurrentVisualizer();

signals:

	void currentVisualizerChanged();

        
private slots:

    void removeAllSeries();    

    //! \param id Id bieżącego wizualizatora.
    void setCurrentVisualizer(int idx);
    //! 
    void fillSourcesMenu();
    //!
    void sourceSelected();
        
private:

    void innerRemoveAllSeries();

    void clearDataSeries();

    //! Inicjalizacja wizualizatora.
    void init();

    void clearSources();

    void clearCurrentVisualizerWidget();

    void addSourceDefaultAction();
};

}

#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
