#ifndef WIDGET_INTERFACE_H
#define WIDGET_INTERFACE_H

#include <osgViewer/Scene>
#include <core/IServiceManager.h>

class IWidget
{
public:
    virtual void SetData(osgViewer::Scene *scene) = 0;
    virtual void SetData(osgViewer::Scene *scene, IServiceManager *pServiceManager) = 0;
    virtual void* GetWidget() = 0;
protected:
private:
};


#endif //WIDGET_INTERFACE_H