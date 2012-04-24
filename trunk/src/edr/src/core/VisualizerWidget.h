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
#include <QtGui/QDockWidget>
#include "Visualizer.h"
#include "VisualizerManager.h"
#include <core/PluginCommon.h>
#include "EDRDockWidget.h"
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <core/IEDRTitleBar.h>

//! Widget wizualizacyjny.
class VisualizerWidget : public EDRDockWidget
{
    Q_OBJECT

public:

    typedef std::pair<QObject*, IEDRTitleBar::SideType> VisualizerTitleBarElement;
    typedef std::vector<VisualizerTitleBarElement> VisualizerTitleBarElements;

private:

    struct InnerVisualizerElement
    {
        InnerVisualizerElement() : visible(false), object(nullptr), side(IEDRTitleBar::Left)
        {

        }

        InnerVisualizerElement(bool visible, QObject* object, IEDRTitleBar::SideType side)
            : visible(visible), object(object), side(side)
        {

        }

        bool visible;
        QObject* object;
        IEDRTitleBar::SideType side;
    };

    //typedef std::map<core::VisualizerTimeSeriePtr, VisualizerChannelPtr> TimelineChannels;
    typedef std::map<QObject*, InnerVisualizerElement> InnerVisualizerElements;

private:    

    //-------------------------- Old VisualizerTitleBar fields -------------------------------------------
    QAction *actionNone;
    QLabel *label;
    QComboBox *comboType;

    //! Bie¿¹cy wizualizator.
    VisualizerPtr visualizer;
    //customowe elementu titlebara dla danego wizualizatora
    InnerVisualizerElements visualizerCommonElements;
    ActionsGroupManager visualizerCommonElementsOrder;
    ActionsGroupManager visualizerImplementationCustomElements;

    //! Menu do wyboru Ÿróde³.
    QMenu* menuSource;

    QWidget * visualizerWidget;


    //TimelineChannels timelineChannels;

    std::map<core::ObjectWrapperConstPtr, core::VisualizerSeriePtr > currentSeriesData;
    std::map<core::TypeInfo, std::set<core::ObjectWrapperConstPtr> > groupedSeriesData;

    std::pair<QAction*, core::ObjectWrapperConstPtr> lastSerie;
	std::map<core::ObjectWrapperConstPtr, const void *> timelineChannels;

public:
    //! Zeruj¹cy konstruktor.
    //! \param parent
    //! \param flags
    VisualizerWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(UniqueID visualizerID, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(const VisualizerPtr& source, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
        
            
    //! Zapewnia mo¿liwoœæ kasowanie widgeta wizualizatora przez jego implementacjê.
    ~VisualizerWidget();
        
public:

    //-------------------------- Old VisualizerTitleBar methods -------------------------------------------
    //! Dodaje wizualizator do listy obs³ugiwanych.
    void addVisualizer(const QString& label, UniqueID id);
    //! Dodaje wizualizator do listy obs³ugiwanych.
    void addVisualizer(const QIcon& icon, const QString& label, UniqueID id);

    //! \return Liczba zarejestrowanych wizualizatorów.
    int getNumVisualizers() const;
    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan pocz¹tkowy.
    void clearCurrentVisualizer();

    //! \return Bie¿¹cy wizualizator.
    inline VisualizerPtr getCurrentVisualizer()
    {
        return visualizer;
    }

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
    const ActionsGroupManager & getVisualizerActionsManager() const;
    const ActionsGroupManager & getGenericVisualizerActionsManager() const;
        
private slots:

    void removeAllSeries();    

    //! \param id Id bie¿¹cego wizualizatora.
    void setCurrentVisualizer(UniqueID id);
    //! \param visualizer Bie¿¹cy wizualizator.
    void setCurrentVisualizer(const VisualizerPtr& visualizer);
    //! \param idx Indeks bie¿¹cego wizualizatora.
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

#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__