#ifndef QT_WIDGET_H
#define QT_WIDGET_H

#include "../include/QtWidgetInterface.h"
class OsgControlWidget;

class QtWidget: public IQtWidget
{
public:
    QtWidget();
    ~QtWidget();

    virtual void SetData(osgViewer::Scene *scene);
    virtual void SetData(osgViewer::Scene *scene, IServiceManager *pServiceManager);
    virtual void* GetWidget();
    void Clear(); 

private:
    OsgControlWidget* _controlWidget;

};

#endif //QT_WIDGET_H