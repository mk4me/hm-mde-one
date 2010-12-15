#ifndef GRID_WIDGET_H
#define GRID_WIDGET_H

#include <QtGui/QWidget>
#include <osg/Node>
#include <core/IService.h>
#include "ui_GridWidget.h"

class SceneGraphWidget: public QWidget, Ui::SceneGraphWidget
{
    Q_OBJECT
public:
    SceneGraphWidget(void);

    void addService(IServicePtr service);
    void setSceneGraph(osg::Node* root); 
    
public slots: 
    void serviceSelectionChanged(int index);
    void refreshButtonClicked();

private:
    void setCurrentService( int index );
    void clearTreeWidget(); 
};


#endif   //GRID_WIDGET_H
