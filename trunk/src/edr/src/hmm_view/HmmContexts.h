/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   11:35
	filename: 	HmmContexts.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMCONTEXTS_H__
#define HEADER_GUARD_HMM__HMMCONTEXTS_H__

#include <boost/tuple/tuple.hpp>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>
#include "FlexiTabWidget.h"
#include "IAppUsageContextManager.h"

class HMMEmptyContext : public IAppUsageContext
{
public:
	HMMEmptyContext() {}
	virtual ~HMMEmptyContext() {}
public:

	virtual void activateContext(QWidget * contextWidget) {}

	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh) {}
};

class HMMDataContext : public HMMEmptyContext
{

};

class TreeRefresher;

class HMMAnalysisContext : public IAppUsageContext
{
public:

	HMMAnalysisContext(TreeRefresher * refresher);

	virtual void activateContext(QWidget * contextWidget);

	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);

private:

	TreeRefresher * refresher;
};

class HMMReportContext : public HMMEmptyContext
{

};

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
class HmmTreeItem;
class HMMTreeItemUsageContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    HMMTreeItemUsageContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);
    // IAppUsageContext
public:

    virtual void activateContext(QWidget * contextWidget);

    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

public slots:
    void refresh();

private slots:
    void itemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    void recreateFlexiSectionWidget(QWidget* flexiSection, HmmTreeItem* helper);

private:
    FlexiTabWidget * flexiTabWidget;
    QWidget* flexiSection;
    FlexiTabWidget::GUIID groupID;
    HmmMainWindow* hmm;
};
typedef core::shared_ptr<HMMTreeItemUsageContext> HMMTreeItemUsageContextPtr;
typedef core::shared_ptr<const HMMTreeItemUsageContext> HMMTreeItemUsageContextConstPtr;

class RaportsThumbnailsContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    RaportsThumbnailsContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);
public:

    virtual void activateContext(QWidget * contextWidget);
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private slots:
    void createRaport();

private:
    FlexiTabWidget * flexiTabWidget;
    QWidget* flexiSection;
    FlexiTabWidget::GUIID groupID;
    HmmMainWindow* hmm;
    QPlainTextEdit* projectName;
    QComboBox* projectTemplate;
    QComboBox* cssCombo;
};
typedef core::shared_ptr<RaportsThumbnailsContext> RaportsThumbnailsContextPtr;
typedef core::shared_ptr<const RaportsThumbnailsContext> RaportsThumbnailsContextConstPtr;

class RaportsTabContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    RaportsTabContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);

public:
    virtual void activateContext(QWidget * contextWidget);
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    boost::tuple<QWidget*, QLayout*, QLayout*> createTwoLineWidget();
    void placeObjects( const QList<QObject*> &editList, QLayout* lowerLayout, QLayout* upperLayout, bool actionsOnTop = false );

private:
    FlexiTabWidget * flexiTabWidget;
    QWidget* editSection;
    QWidget* textSection;
    QWidget* fileSection;
    FlexiTabWidget::GUIID groupID;
    HmmMainWindow* hmm;
};
typedef core::shared_ptr<RaportsTabContext> RaportsTabContextPtr;
typedef core::shared_ptr<const RaportsTabContext> RaportsTabContextConstPtr;

#endif
