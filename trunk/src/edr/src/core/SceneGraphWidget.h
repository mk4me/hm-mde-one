#ifndef GRID_WIDGET_H
#define GRID_WIDGET_H

#include <osg/Node>
#include <QtGui/QWidget>
#include <core/IService.h>
#include <core/IServiceManager.h>
#include "ui_SceneGraphWidget.h"
#include "Visualizer.h"

class SceneGraphWidget: public QWidget, Ui::SceneGraphWidget
{
    Q_OBJECT
public:
    //!
    SceneGraphWidget();

    //! Dodaje wizualizator do listy.
    void addVisualizer(Visualizer* visualizer);
    //! Usuwa wizualizator z listy.
    void removeVisualizer(Visualizer* visualizer);

    //! Wype�nia drzewo sceny na podstawie zadanego korzenia.
    //! \param root
    void setSceneGraph(osg::Node* root); 
    
public slots: 
    void setCurrentVisualizer(int index);
    void refreshButtonClicked();

private:
//     //! Ustawia drzewo sceny na podstawie wybranej us�ugi.
//     void setCurrentService( int index );
    //! Czy�ci zawarto�� drzewa sceny.
    void clearTreeWidget(); 
};


#endif   //GRID_WIDGET_H
