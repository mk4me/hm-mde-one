#include "SummaryWindow.h"
#include "TreeItemHelper.h"
#include "HmmMainWindow.h"
#include "AnalisisWidget.h"
#include "VisualizerWidget.h"
#include "TreeItemHelper.h"
#include <plugins/subject/ISubject.h>

void SummaryWindow::initialize()
{

}

void SummaryWindow::display( const std::vector<TreeItemHelper*>& helpers )
{
    QString text;
    for (auto it = helpers.begin(); it != helpers.end(); it++) {
        PluginSubject::MotionConstPtr motion = (*it)->getMotion();
        if (motion) {
            PluginSubject::SessionConstPtr session = motion->getSession();
            PluginSubject::SubjectConstPtr subject = session->getSubject();

            text += QString::fromStdString(motion->getLocalName()) + "\n";
            text += QString::fromStdString(session->getLocalName()) + "\n";
            text += QString::fromStdString(subject->getName());
        }
        TreeWrappedItemHelper* wrapped = dynamic_cast<TreeWrappedItemHelper*>(*it);
        if (wrapped) {
            text = QString::fromStdString(wrapped->getWrapper()->getName()) + "\n" + text;
        } 
        text += "\n";
    }
    hmm->getAnalisis()->SummaryWindowWidget->setText(text);
}


SummaryWindowController::SummaryWindowController( SummaryWindowPtr sw, HmmMainWindow* hmm  ) :
    summary(sw),
    hmm(hmm)
{

}

void SummaryWindowController::onTreeItemSelected( QTreeWidgetItem* item, int column )
{
    SummaryWindow::HelpersCollection collection;
    TreeItemHelper* helper = dynamic_cast<TreeItemHelper*>(item);
    if (helper) {
        collection.push_back(helper);
    }
    summary->display(collection);
}

void SummaryWindowController::onVisualizator( VisualizerWidget* visualizatorWidget )
{
    VisualizerPtr visualizer = visualizatorWidget->getCurrentVisualizer();
    
    SummaryWindow::HelpersCollection collection;
    for (auto it = hmm->items2Descriptions.begin(); it != hmm->items2Descriptions.end(); it++) {
    
        if (it->second.visualizerWidget == visualizatorWidget) {
            collection.push_back(it->first);
        }
    }
    summary->display(collection);
}
