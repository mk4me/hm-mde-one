/********************************************************************
	created:  2011/03/10
	created:  10:3:2011   17:15
	filename: VisualizerWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGET_H__

#include <map>
#include "Visualizer.h"
#include "CoreDockWidget.h"
#include "CoreTitleBar.h"

class QLabel;
class QComboBox;

namespace coreUI {

//! Widget wizualizacyjny.
class VisualizerWidget : public CoreDockWidget
{
    Q_OBJECT

public:

    typedef std::pair<QObject*, CoreTitleBar::SideType> VisualizerTitleBarElement;
    typedef std::vector<VisualizerTitleBarElement> VisualizerTitleBarElements;

private:

    struct InnerVisualizerElement
    {
        InnerVisualizerElement() : visible(false), object(nullptr), side(CoreTitleBar::Left)
        {

        }

        InnerVisualizerElement(bool visible, QObject* object, CoreTitleBar::SideType side)
            : visible(visible), object(object), side(side)
        {

        }

        bool visible;
        QObject* object;
        CoreTitleBar::SideType side;
    };

    //typedef std::map<core::VisualizerTimeSeriePtr, VisualizerChannelPtr> TimelineChannels;
    typedef std::map<QObject*, InnerVisualizerElement> InnerVisualizerElements;

private:    

    //-------------------------- Old VisualizerTitleBar fields -------------------------------------------
    QAction *actionNone;
    QLabel *label;
    QComboBox *comboType;

    //! Bieżący wizualizator.
    core::VisualizerPtr visualizer;
    //customowe elementu titlebara dla danego wizualizatora
    InnerVisualizerElements visualizerCommonElements;
    core::ActionsGroupManager visualizerCommonElementsOrder;
    core::ActionsGroupManager visualizerImplementationCustomElements;

    //! Menu do wyboru źródeł.
    QMenu* menuSource;

    QWidget * visualizerWidget;
	QWidget * visualizerWidgetContainer;


    //TimelineChannels timelineChannels;

    std::map<core::ObjectWrapperConstPtr, plugin::VisualizerSeriePtr > currentSeriesData;
    std::map<core::TypeInfo, std::set<core::ObjectWrapperConstPtr> > groupedSeriesData;

    std::pair<QAction*, core::ObjectWrapperConstPtr> lastSerie;
	std::map<core::ObjectWrapperConstPtr, const void *> timelineChannels;

	bool autoRefreshInputs_;

public:
    //! Zerujący konstruktor.
    //! \param parent
    //! \param flags
    VisualizerWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0, bool autoRefreshInputs = true);
    //!
    VisualizerWidget(UniqueID visualizerID, QWidget* parent = nullptr, Qt::WindowFlags flags = 0, bool autoRefreshInputs = true);
    //!
    VisualizerWidget(const core::VisualizerPtr& source, QWidget* parent = nullptr, Qt::WindowFlags flags = 0, bool autoRefreshInputs = true);
        
            
    //! Zapewnia możliwość kasowanie widgeta wizualizatora przez jego implementację.
    ~VisualizerWidget();
        
public:

    //-------------------------- Old VisualizerTitleBar methods -------------------------------------------
    //! Dodaje wizualizator do listy obsługiwanych.
    void addVisualizer(const QString& label, UniqueID id);
    //! Dodaje wizualizator do listy obsługiwanych.
    void addVisualizer(const QIcon& icon, const QString& label, UniqueID id);

    //! \return Liczba zarejestrowanych wizualizatorów.
    int getNumVisualizers() const;
    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan początkowy.
    void clearCurrentVisualizer();

    //! \return Bieżący wizualizator.
    inline core::VisualizerPtr getCurrentVisualizer()
    {
        return visualizer;
    }

	void setAutoRefreshInputs(bool autoRefresh);
	bool autoRefreshInputs() const;

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

    void getVisualizerTitleBarElements(VisualizerTitleBarElements & titleBarElements) const;
    const core::ActionsGroupManager & getVisualizerActionsManager() const;
    const core::ActionsGroupManager & getGenericVisualizerActionsManager() const;
        
private slots:

    void removeAllSeries();    

    //! \param id Id bieżącego wizualizatora.
    void setCurrentVisualizer(UniqueID id);
    //! \param visualizer Bieżący wizualizator.
    void setCurrentVisualizer(const core::VisualizerPtr& visualizer);
    //! \param idx Indeks bieżącego wizualizatora.
    void setCurrentVisualizer(int idx);

    //! 
    void fillSourcesMenu();
    //!
    void sourceSelected();
        
private:

    void innerRemoveAllSeries();

    void clearDataSeries();

    //-------------------------- Old VisualizerTitleBarComboPick methods -------------------------------------------
    void retranslateUi(QWidget * visualizerWidget);
    //! Inicjalizacja wizualizatora.
    void init();
    //-------------------------- Old VisualizerTitleBarComboPick methods -------------------------------------------
    std::string getLabel( const core::ObjectWrapperConstPtr& object, bool noSource );

    void clearSources();

    void clearCurrentVisualizerWidget();

    void addSourceDefaultAction();
};

}

#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
