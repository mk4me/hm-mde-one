#ifndef GRID_WIDGET_H
#define GRID_WIDGET_H

#include <osg/Node>
#include <QtGui/QWidget>
#include <core/IService.h>
#include <core/IServiceManager.h>
#include "ui_SceneGraphWidget.h"

class SceneGraphWidget: public QWidget, Ui::SceneGraphWidget
{
    Q_OBJECT
public:
    //!
    SceneGraphWidget();

    //! Dodaje us�ugi do listy us�ug.
    //! \param manager
    void addServices(IServiceManager* manager);
    //! Dodaje us�ug� do listy us�ug.
    //! \param service
    void addService(IServicePtr service);
    //! Wype�nia drzewo sceny na podstawie zadanego korzenia.
    //! \param root
    void setSceneGraph(osg::Node* root); 
    
public slots: 
    void serviceSelectionChanged(int index);
    void refreshButtonClicked();

private:
    //! Ustawia drzewo sceny na podstawie wybranej us�ugi.
    void setCurrentService( int index );
    //! Czy�ci zawarto�� drzewa sceny.
    void clearTreeWidget(); 
};


#endif   //GRID_WIDGET_H
