#ifndef IQT_WIDGET_INTERFACE_H
#define IQT_WIDGET_INTERFACE_H

#include <core/WidgetInterface.h>

class IQtWidget: public IWidget
{
public:
    virtual void SetData(osg::Node *scene) = 0;
    virtual void SetData(osg::Node *scene, IServiceManager *pServiceManager) = 0;
    virtual void* GetWidget() = 0;
protected:
private:
};



#endif //IQT_WIDGET_INTERFACE_H