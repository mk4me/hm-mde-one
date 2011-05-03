/********************************************************************
	created:  2011/03/10
	created:  10:3:2011   17:15
	filename: VisualizerWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGET_H__

#include <QtGui/QDockWidget>
#include "Visualizer.h"
#include "VisualizerManager.h"
#include "VisualizerTitleBar.h"
#include <core/PluginCommon.h>
#include "EDRDockWidget.h"

//! Widget wizualizacyjny.
class VisualizerWidget : public EDRDockWidget
{
    Q_OBJECT

private:

    //-------------------------- Old VisualizerTitleBar fields -------------------------------------------
    QAction *actionSplitVertically;
    QAction *actionSplitHorizontally;
    QLabel *label;
    QComboBox *comboType;

    QToolButton *buttonSplitV;
    QToolButton *buttonSplitH;

    //! Bie��cy wizualizator.
    VisualizerPtr visualizer;
    //customowe elementu titlebara dla danego wizualizatora
    std::vector<QObject*> visualizerCustomElements;

    //-------------------------- Old VisualizerTitleBarComboPick fields -------------------------------------------
    QToolButton *buttonPin;
    QToolButton *buttonSource;

    //! Menu do wyboru �r�de�.
    QMenu* menuSource;
    //!
    QMenu* menuPin;
    //!
    QActionGroup* groupMenuPin;
    //! Grupa akcji dla wyobru �r�de�.
    QActionGroup* groupSources;
    //!
    int currentSlot;

    QWidget * visualizerWidget;

public:
    //! Zeruj�cy konstruktor.
    //! \param parent
    //! \param flags
    VisualizerWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(UniqueID visualizerID, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(const VisualizerPtr& source, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
        
            
    //! Zapewnia mo�liwo�� kasowanie widgeta wizualizatora przez jego implementacj�.
    ~VisualizerWidget();
        
public:

    //-------------------------- Old VisualizerTitleBar methods -------------------------------------------
    //! Dodaje wizualizator do listy obs�ugiwanych.
    void addVisualizer(const QString& label, UniqueID id);
    //! Dodaje wizualizator do listy obs�ugiwanych.
    void addVisualizer(const QIcon& icon, const QString& label, UniqueID id);

    //! \return Liczba zarejestrowanych wizualizator�w.
    int getNumVisualizers() const;
    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan pocz�tkowy.
    void clearCurrentVisualizer();

    //! \return Bie��cy wizualizator.
    inline VisualizerPtr getCurrentVisualizer()
    {
        return visualizer;
    }

protected:
    virtual void closeEvent (QCloseEvent *event);
        
private slots:
    //! \param Czy zamieni� pasek tytu�owy na systemowy?
    //void setReplaceTitleBar(bool replace);
    //! \param Czy zamieni� pasek tytu�owy na systemowy?
    //void queueReplaceTitleBar(bool replace);
    //!
    void splitHorizontally();
    //!
    void splitVertically();
    //!
    void split(Qt::Orientation orientation);

    //-------------------------- Old VisualizerTitleBar slots -------------------------------------------
    //! \param id Id bie��cego wizualizatora.
    void setCurrentVisualizer(UniqueID id);
    //! \param visualizer Bie��cy wizualizator.
    void setCurrentVisualizer(const VisualizerPtr& visualizer);
    //! \param idx Indeks bie��cego wizualizatora.
    void setCurrentVisualizer(int idx);

    //-------------------------- Old VisualizerTitleBarComboPick slots -------------------------------------------
    void setCurrentSlot( int idx );
    //void setCurrentSlot(const core::TypeInfo & slot);

    void setCurrentSource( int idx );

private slots:

    //-------------------------- Old VisualizerTitleBarComboPick slots -------------------------------------------
    //! 
    void fillSourcesMenu();
    //!
    void sourceSelected();
    //!
    void pinSelected();
        
private:

    //-------------------------- Old VisualizerTitleBarComboPick methods -------------------------------------------
    int getCurrentSlot() const;
    void clearVisualizerSources();

    void retranslateUi(QWidget * visualizerWidget);

    //! Inicjalizacja wizualizatora.
    void init();

    //-------------------------- Old VisualizerTitleBarComboPick methods -------------------------------------------
    QString getLabel( const core::ObjectWrapperConstPtr& object, bool noSource );

    void clearSources();

    void clearSlots();

    void clearCurrentVisualizerWidget();

    void addSourceDefaultAction();

    void addSlotDefaultAction();
};


#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__