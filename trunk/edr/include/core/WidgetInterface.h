#ifndef WIDGET_INTERFACE_H
#define WIDGET_INTERFACE_H

#include <osgViewer/Scene>

class IServiceManager;

class IWidget
{
public:
    virtual void SetData(osg::Node *scene) = 0;
    virtual void SetData(osg::Node *scene, IServiceManager *pServiceManager) = 0;
    virtual void* GetWidget() = 0;
protected:
private:
};


#endif //WIDGET_INTERFACE_H