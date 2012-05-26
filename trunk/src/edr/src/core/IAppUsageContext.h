/********************************************************************
    created:  2011/12/16
    created:  16:12:2011   8:47
    filename: IAppUsageContext.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IAPPUSAGECONTEXT_H__
#define HEADER_GUARD___IAPPUSAGECONTEXT_H__

#include <set>
#include <map>
//#include <QtGui/QWidget>


#include <core/SmartPtr.h>


class QWidget;
class IAppUsageContextManager;

class IAppUsageContext;
typedef core::shared_ptr<IAppUsageContext> AppUsageContextPtr;
typedef core::weak_ptr<IAppUsageContext> AppUsageContextWeakPtr;
typedef core::shared_ptr<const IAppUsageContext> AppUsageContextConstPtr;
typedef core::weak_ptr<const IAppUsageContext> AppUsageContextConstWeakPtr;


class IAppUsageContext
{

    friend class IAppUsageContextManager;

public:

    typedef std::map<QString, std::vector<QObject*>> OperationGroups;

private:

    typedef std::set<AppUsageContextPtr> ChildContexts;

protected:
    IAppUsageContext() : active(false), currentContextWidget(nullptr), destroyed_(false) {}

public:
    virtual ~IAppUsageContext() {
        clearContext();
    }

    AppUsageContextPtr parentContext() const
    {
        return parent.lock();
    }

    QWidget * getCurrentContextWidget() const
    {
        return currentContextWidget;
    }

    bool isActive() const
    {
        return active;
    }

    bool isCurrentContextWidgetDestroyed() const
    {
        return destroyed_;
    }

    void setCurrentContextWidgetDestroyed(bool destroyed)
    {
        destroyed_ = destroyed;
    }

private:
    void activate(QWidget * contextWidget = nullptr)
    {
        if(active == true){
            throw std::runtime_error("Trying to activate already active context");
        }

        if(contextWidget != nullptr && contextWidgets.find(contextWidget) == contextWidgets.end()){
            throw std::runtime_error("Trying to activate context from unsupported widget");
        }

        active = false;
        activateContext(contextWidget);
        active = true;
        currentContextWidget = contextWidget;
    }

    bool tryActivate(QWidget * contextWidget = nullptr)
    {
        bool ret = true;
        try{
            activate(contextWidget);
        }catch(...){
            tryDeactivate();
            ret = false;
        }

        return ret;
    }

    void deactivate(QWidget * nextContextWidget = nullptr, bool refresh = false)
    {
        if(active == false){
            throw std::runtime_error("Trying to deactivate inactive context");
        }

        deactivateContext(nextContextWidget, refresh);
        active = false;
        currentContextWidget = nullptr;
    }

    bool tryDeactivate(QWidget * nextContextWidget = nullptr, bool refresh = false)
    {
        bool ret = true;
        try{
            deactivate(nextContextWidget, refresh);
        }catch(...){
            ret = false;
        }

        return ret;
    }

    void registerContextWidget(QWidget * widget)
    {
        contextWidgets.insert(widget);
        onRegisterContextWidget(widget);
    }

    void unregisterContextWidget(QWidget * widget)
    {
        contextWidgets.erase(widget);
        onUnregisterContextWidget(widget);
    }

    const ChildContexts & registeredChildContexts() const
    {
        return childContexts;
    }

    const std::set<QWidget*> & registeredContextWidgets() const
    {
        return contextWidgets;
    }

    void clearContextWidgets()
    {
        for(auto it = contextWidgets.begin(); it != contextWidgets.end(); it++){
            onUnregisterContextWidget(*it);
        }

        contextWidgets.swap(std::set<QWidget*>());
    }

    void clearChildContexts()
    {
        childContexts.swap(ChildContexts());
    }

    void clearContext()
    {
        clearContextWidgets();
        clearChildContexts();
    }

private:

    virtual void activateContext(QWidget * contextWidget) = 0;
    virtual void deactivateContext(QWidget * nextContextWidget = nullptr, bool refresh = false) = 0;

    virtual void onRegisterContextWidget(QWidget * widget) {}
    virtual void onUnregisterContextWidget(QWidget * widget) {}

private:
    bool active;
    bool destroyed_;
    QWidget * currentContextWidget;
    std::set<QWidget*> contextWidgets;
    AppUsageContextWeakPtr parent;
    ChildContexts childContexts;
};

#endif HEADER_GUARD___IAPPUSAGECONTEXT_H__