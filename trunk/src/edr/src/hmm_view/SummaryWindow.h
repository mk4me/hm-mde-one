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
class TreeItemHelper;

class SummaryWindow
{
public:
    SummaryWindow(HmmMainWindow* hmm) : hmm(hmm) {}
	virtual ~SummaryWindow() {}

public:
    typedef std::vector<TreeItemHelper*> HelpersCollection;

public:
    void initialize();
    void display(const HelpersCollection& helpers);

private:
    HmmMainWindow* hmm;
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
