/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   14:35
    filename: EDRTitleBar.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRTITLEBAR_H__
#define HEADER_GUARD_CORE__EDRTITLEBAR_H__

#include <core/ui_EDRTitleBar.h>
#include <QtGui/QWidget>
#include <core/IEDRTitleBar.h>
#include <QtGui/QDockWidget>
#include <map>

class EDRDockWidget;

class EDRTitleBar : public QWidget, public Ui::EDRTitleBar, public IEDRTitleBar
{
    Q_OBJECT

private:
    typedef std::pair<SideType, QWidget*> ObjectDescriptor;
    typedef std::map<QObject*, ObjectDescriptor> ObjectData;
    typedef std::map<QWidget*, QObject*> WidgetObjects;

public:
    //! \param parent
    EDRTitleBar(QWidget* parent = nullptr);
    virtual ~EDRTitleBar();

public:

    virtual void addObject(QObject * object, SideType side);
    virtual void removeObject(QObject * object);
    bool isCloseButtonVisible() const;
    bool isFloatButtonVisible() const;
    bool isTitleVisible() const;
    QString getTitle() const;
    
public slots:
    virtual void clearSide(SideType side);
    virtual void clear();

    void setCloseButtonVisible(bool visible);
    void setFloatButtonVisible(bool visible);
    void setTitleVisible(bool visible);
    void setTitle(const QString & title);

    void refreshFeatures(QDockWidget::DockWidgetFeatures features);

protected:
    virtual void paintEvent(QPaintEvent *paintEvent);

private:
    QFrame * getSideFrame(SideType side) const;

private:

    ObjectData objectData;
    WidgetObjects widgetObjects;
};

EDRTitleBar * supplyWithEDRTitleBar(EDRDockWidget * dockWidget, bool refresh = true);

#endif  //  HEADER_GUARD_CORE__EDRTITLEBAR_H__
