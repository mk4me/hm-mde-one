/********************************************************************
    created:  2011/03/22
    created:  22:3:2011   19:37
    filename: VisualizerTitleBarContextBased.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERTITLEBARCONTEXTBASED_H__
#define HEADER_GUARD_CORE__VISUALIZERTITLEBARCONTEXTBASED_H__

#include "VisualizerTitleBar.h"
#include "ui_VisualizerTitleBarComboPick.h"


class VisualizerTitleBarComboPick : public QWidget, private Ui::VisualizerTitleBarComboPick
{
    Q_OBJECT
private:
    //! Menu do wyboru Ÿróde³.
    QMenu* menuSource;
    //! Grupa akcji dla wyobru Ÿróde³.
    QActionGroup* groupSources;
    //! Wizualizator.
    VisualizerPtr visualizer;

public:
    //! \param parent
    VisualizerTitleBarComboPick( QWidget* parent = nullptr );

public:

    int getCurrentSlot() const;
    void clear();

public slots:
    void setCurrentSlot( int idx );
    void setCurrentVisualizer( const VisualizerPtr& visualizer );

private slots:
    //! 
    void fillSourcesMenu();
    //!
    void sourceSelected();

private:
    QString getLabel( const core::ObjectWrapperConstPtr& object, bool noSource );

    //!
    void fillSourcesMenu(QMenu* menu, QActionGroup* group, int slotNo);
};


#endif  // HEADER_GUARD_CORE__VISUALIZERTITLEBARCONTEXTBASED_H__