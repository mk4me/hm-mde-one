#ifndef IWx_WIDGET_INTERFACE_H
#define IWx_WIDGET_INTERFACE_H

class IWidget;

class IWxWidget: public IWidget
{
public:
    virtual void SetData(osg::Node *scene) = 0;
	virtual void* GetWidget() = 0;
protected:
private:
};



#endif //IWx_WIDGET_INTERFACE_H