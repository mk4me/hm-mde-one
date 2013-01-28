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

//! Widget wizualizacyjny.
class CoreVisualizerWidget : public QWidget
{
    Q_OBJECT

private:    

    //-------------------------- Old VisualizerTitleBar fields -------------------------------------------
    QAction *actionNone;
    QLabel *label;
    QComboBox *comboType;

    //! Bieżący wizualizator.
    core::VisualizerPtr visualizer;

    //! Menu do wyboru źródeł.
    QMenu* menuSource;

    QWidget * visualizerWidget;
	QWidget * visualizerWidgetContainer;

    std::map<core::ObjectWrapperConstPtr, plugin::VisualizerSeriePtr > currentSeriesData;
    std::map<core::TypeInfo, std::set<core::ObjectWrapperConstPtr> > groupedSeriesData;

    std::pair<QAction*, core::ObjectWrapperConstPtr> lastSerie;
	std::map<core::ObjectWrapperConstPtr, const void *> timelineChannels;

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
    void addVisualizer(core::VisualizerPtr visualizer);

	//! Dodaje wizualizator do listy obsługiwanych.
	void removeVisualizer(core::VisualizerPtr visualizer);

    //! \return Liczba zarejestrowanych wizualizatorów.
    int getNumVisualizers() const;
    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan początkowy.
    void clearCurrentVisualizer();

    //! \return Bieżący wizualizator.
    core::VisualizerPtr getCurrentVisualizer();

public slots:

    void splitHorizontally();
    //!
    void splitVertically();
    //!
    void split(Qt::Orientation orientation);

    void setSourceVisible(bool visible);
    void setVisualizerIconVisible(bool visible);
    void setVisualizerSwitchEnable(bool enable);
    void setVisualizerSwitchVisible(bool visible);

public:

    bool isSourceVisible() const;  
    bool isVisualizerIconVisible() const;    
    bool isVisualizerSwichEnable() const;    
    bool isVisualizerSwichVisible() const;
        
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
