#ifndef QT_WIDGET_H
#define QT_WIDGET_H

#include <core/QtWidgetInterface.h>
class OsgControlWidget;

class QtWidget: public IQtWidget
{
public:
    QtWidget();
    ~QtWidget();

    virtual void SetData(osg::Node *scene);
    virtual void SetData(osg::Node *scene, IServiceManager *pServiceManager);
    virtual void* GetWidget();
    void Clear(); 

private:
    OsgControlWidget* _controlWidget;

};

#endif //QT_WIDGET_H