#ifndef IQT_WIDGET_INTERFACE_H
#define IQT_WIDGET_INTERFACE_H

#include <core/WidgetInterface.h>

class IQtWidget: public IWidget
{
public:
    virtual void SetData(osgViewer::Scene *scene) = 0;
    virtual void SetData(osgViewer::Scene *scene, IServiceManager *pServiceManager) = 0;
    virtual void* GetWidget() = 0;
protected:
private:
};



#endif //IQT_WIDGET_INTERFACE_H