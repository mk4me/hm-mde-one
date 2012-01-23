/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   11:35
	filename: 	HmmContexts.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMCONTEXTS_H__
#define HEADER_GUARD_HMM__HMMCONTEXTS_H__

#include "FlexiTabWidget.h"
#include "IAppUsageContextManager.h"


class HMMVisualizerUsageContext : public IAppUsageContext
{
public:
    HMMVisualizerUsageContext(FlexiTabWidget * flexiTabWidget);
    // IAppUsageContext
public:

    virtual void activateContext(QWidget * contextWidget);

    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);

    virtual void onRegisterContextWidget(QWidget * contextWidget);
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    FlexiTabWidget * flexiTabWidget;
    FlexiTabWidget::GUIID visualizerGroupID;
    std::set<FlexiTabWidget::GUIID> visualizerSectionsIDs;

    std::map<QWidget*, std::map<QString, QWidget *>> visualizersData;
};


class HmmMainWindow;
class TreeItemHelper;
class HMMTreeItemUsageContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    HMMTreeItemUsageContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);
    // IAppUsageContext
public:

    virtual void activateContext(QWidget * contextWidget);

    //virtual void deactivateContext(QWidget * contextWidget);
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

    //void setActiveTreeItem(TreeItemHelper* helper);

private slots:
    void itemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void refresh(QAction* dummy = nullptr);

private:
    void recreateFlexiSectionWidget(QWidget* flexiSection, TreeItemHelper* helper);

private:
    FlexiTabWidget * flexiTabWidget;
    QWidget* flexiSection;
    FlexiTabWidget::GUIID groupID;
    HmmMainWindow* hmm;
};
typedef core::shared_ptr<HMMTreeItemUsageContext> HMMTreeItemUsageContextPtr;
typedef core::shared_ptr<const HMMTreeItemUsageContext> HMMTreeItemUsageContextConstPtr;

#endif
