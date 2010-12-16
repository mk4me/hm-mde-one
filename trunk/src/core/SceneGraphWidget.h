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

    //! Dodaje us³ugi do listy us³ug.
    //! \param manager
    void addServices(IServiceManager* manager);
    //! Dodaje us³ugê do listy us³ug.
    //! \param service
    void addService(IServicePtr service);
    //! Wype³nia drzewo sceny na podstawie zadanego korzenia.
    //! \param root
    void setSceneGraph(osg::Node* root); 
    
public slots: 
    void serviceSelectionChanged(int index);
    void refreshButtonClicked();

private:
    //! Ustawia drzewo sceny na podstawie wybranej us³ugi.
    void setCurrentService( int index );
    //! Czyœci zawartoœæ drzewa sceny.
    void clearTreeWidget(); 
};


#endif   //GRID_WIDGET_H
