/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   18:26
    filename: WorkflowWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___WORKFLOWWIDGET_H__
#define HEADER_GUARD___WORKFLOWWIDGET_H__

#ifdef IN
#undef IN
#endif

#ifdef OUT
#undef OUT
#endif

#include <QtGui/QWidget>
#include "ui_WorkflowWidget.h"
#include <vdfmlib/osgVDFModel.h>
#include <osgui/QOsgWidgets.h>
#include "EDRDockWidget.h"
#include <QtGui/QToolButton>
#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <boost/function.hpp>
#include "WorkflowItemEncapsulator.h"
#include "EDRDataFlow.h"

class WorkflowService;
class WorkflowWidget;

class EDRWorkflowWidget;

class WorkflowCustomQOSGWidget : public osgui::QOsgDefaultWidget
{
    Q_OBJECT;
    friend class EDRWorkflowWidget;
public:
    WorkflowCustomQOSGWidget(QWidget * parent = nullptr, const osg::GraphicsContext::Traits* traits = nullptr, Qt::WindowFlags f = 0)
        : osgui::QOsgDefaultWidget(parent, traits, f), workflowWidget(nullptr)
    {

    }

    ~WorkflowCustomQOSGWidget()
    {

    }

protected slots:

    void tryAddNode();

protected:

    virtual void mousePressEvent(QMouseEvent * event);

private:
    EDRWorkflowWidget * workflowWidget;
    QPoint pos;

};

class WorkflowWidget : public QWidget, public Ui::WorkflowWidget
{
    Q_OBJECT;
    friend class EDRWorkflowWidget;
private:

    typedef std::set<osg::ref_ptr<osgui::StyleEx> > StylesSet;
    typedef boost::function<WorkflowItemPtr(void)> workflowItemBuilder;

private:
    //! Usługa tworząca widget.
    WorkflowService* service;

    //! Widget z widokiem OSG.
    WorkflowCustomQOSGWidget* workflowVDFWidget;
    osg::ref_ptr<osgVDF::osgVDFBaseModel> workflowVDFModel;
    EDRWorkflowWidget* parentWidget;

    std::map<QAction*, workflowItemBuilder> actionsItemBuilders;
    workflowItemBuilder currentItemBuilder;

public:
    //! 
    WorkflowWidget(WorkflowService* service);
    //! 
    virtual ~WorkflowWidget();

private:
    static std::string colorToStr(const osgWidget::Color & color);
    StylesSet generateRequiredStyles();
};

class EDRWorkflowWidget : public EDRDockWidget, public utils::Observer<dflm::Model>
{
    Q_OBJECT;
    friend class WorkflowCustomQOSGWidget;
private:
    typedef std::set<osg::ref_ptr<osgui::StyleEx> > StylesSet;
    typedef boost::function<WorkflowItemPtr(void)> workflowItemBuilder;

private:
    QVBoxLayout* layout;
    QTabWidget * toolbar;

    QAction *actionStart;
    QAction *actionStop;

    core::shared_ptr<EDRDataFlow> model;
    WorkflowCustomQOSGWidget* workflowVDFWidget;
    osg::ref_ptr<osgVDF::osgVDFBaseModel> workflowVDFModel;

    std::map<QAction*, workflowItemBuilder> actionsItemBuilders;
    QAction* currentAction;

    std::map<WorkflowItem*, EDRDockWidget*> logicToUI;

public:
    virtual void update(const dflm::Model* model);

    EDRWorkflowWidget();
    ~EDRWorkflowWidget();

private:
    static std::string colorToStr(const osgWidget::Color & color);
    StylesSet generateRequiredStyles();

    WorkflowItemPtr buildAndInitializeVisualizer(UniqueID id);


    void onNodeDelete(const osgVDF::osgVDFBaseNode * node);
    void onNodeDoubleClick(const osgVDF::osgVDFBaseNode * node, const osgWidget::XYCoord & pos);

protected slots:
    void start(bool s);
    void start();
    void stop();

    void tollbarButoonChanged(bool change);
};

#endif  //  HEADER_GUARD___WORKFLOWWIDGET_H__

