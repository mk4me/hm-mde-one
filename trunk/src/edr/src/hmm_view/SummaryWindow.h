///********************************************************************
//	created:	2012/05/04
//	created:	4:5:2012   13:43
//	filename: 	SummaryWindow.h
//	author:		Wojciech Kniec
//	
//	purpose:	
//*********************************************************************/

#ifndef HEADER_GUARD_HMM__SUMMARYWINDOW_H__
#define HEADER_GUARD_HMM__SUMMARYWINDOW_H__

class HmmMainWindow;
class VisualizerWidget;

#include <plugins/subject/ISubject.h>
#include "TreeItemHelper.h"

class SummaryWindow
{
public:
    SummaryWindow(HmmMainWindow* hmm) : hmm(hmm), tree(nullptr) {}
	virtual ~SummaryWindow() {}

public:
    typedef std::vector<TreeItemHelperPtr> HelpersCollection;

public:
    void initialize();
    void display(const HelpersCollection& helpers);

    void addItem( QString text, QTreeWidgetItem* root );

    QString createDescription( PluginSubject::MotionConstPtr motion ) const;

private:
    HmmMainWindow* hmm;
    QTreeWidget* tree;
};
typedef core::shared_ptr<SummaryWindow> SummaryWindowPtr;
typedef core::shared_ptr<const SummaryWindow> SummaryWindowConstPtr;

class SummaryWindowController : public QObject
{
    Q_OBJECT;

public:
    SummaryWindowController(SummaryWindowPtr sw, HmmMainWindow* hmm);

public slots:
    void onTreeItemSelected(QTreeWidgetItem* item, int column);
    void onVisualizator(VisualizerWidget* visualizatorWidget);

private:
    HmmMainWindow* hmm;
    SummaryWindowPtr summary;
};



#endif
