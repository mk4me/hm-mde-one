#include "SummaryWindow.h"
#include "AnalisisWidget.h"
#include <plugins/newCommunication/IPatient.h>

void SummaryWindow::initialize()
{
    placeholder->setLayout(new QVBoxLayout());
    placeholder->layout()->setContentsMargins(0, 0, 0, 0);
    placeholder->layout()->setMargin(0);
    tree = new QTreeWidget();
    tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tree->setHeaderHidden(true);
    placeholder->layout()->addWidget(tree);
    QTreeWidgetItem* twi = new QTreeWidgetItem();
    tree->addTopLevelItem(twi);
}

void SummaryWindow::display( const core::IHierarchyItemConstWeakPtr& helper )
{
    HelpersCollection hc;
    hc.push_back(helper);
    display(hc);
}

void SummaryWindow::display( const HelpersCollection& helpers )
{
    UTILS_ASSERT(tree != nullptr);

    tree->clear();
    auto size = helpers.size();
    if (size > 0) {
        QTreeWidgetItem* root = new QTreeWidgetItem();
        root->setText(0, QObject::tr("Data Info"));
        QFont font = root->font(0);
        font.setBold(true);
        font.setPointSize(12);
        root->setFont(0, font);
        tree->addTopLevelItem(root);

        for (auto itHelper = helpers.begin(); itHelper != helpers.end(); ++itHelper) {
            
            // TODO
            /*NewMultiserieHelperPtr multiserieHelper = utils::dynamic_pointer_cast<NewMultiserieHelper>(*itHelper);
            if (multiserieHelper) {
                const auto& desc = multiserieHelper->getChartWithDescriptions();
                for (auto it = desc.begin(); it != desc.end(); ++it) {
                    auto motion = it->motion;
                    if (motion) {
						std::string name;
						it->wrapper->tryGetMeta("core/name", name);
                        QString text = QString::fromStdString(name) + createDescription(it->motion);
                        addItem(text, root);
                    }
                }
            } else {*/
                // TODO obsluga motion
                //auto motion = (*itHelper)->getMotion();
                //QString text = motion ? createDescription(motion) : QString();
                QString text;
                core::IHierarchyItemConstPtr hdi = itHelper->lock();
                if (hdi) {
                    text = hdi->getDescription() + "\n" + text;
                }
                addItem(text, root);
            //}
        }

        root->setExpanded(true);
    }
}

void SummaryWindow::addItem( QString text, QTreeWidgetItem* root )
{
    QTreeWidgetItem* itm = new QTreeWidgetItem();
    itm->setText(0, text);
    root->addChild(itm);
}

void SummaryWindow::clear()
{
    tree->clear();
}


SummaryWindowController::SummaryWindowController( SummaryWindowPtr sw, AnalisisModelPtr analysisModel) :
	summary(sw),
    analysisModel(analysisModel)
{

}

//void SummaryWindowController::onTreeItemSelected( QTreeWidgetItem* item, int column )
//{
//    SummaryWindow::HelpersCollection collection;
//    HmmTreeItem* hmmItem = dynamic_cast<HmmTreeItem*>(item);
//    if (hmmItem) {
//        collection.push_back(hmmItem->getHelper());
//    }
//    summary->display(collection);
//}

void SummaryWindowController::onVisualizator( coreUI::CoreVisualizerWidget* visualizatorWidget )
{
    core::VisualizerPtr visualizer = visualizatorWidget->getVisualizer();
    
    SummaryWindow::HelpersCollection collection;
    auto desc = analysisModel->getVisualizerDataDescription(visualizatorWidget->getVisualizer());
    /*for (auto it = hmm->items2Descriptions.begin(); it != hmm->items2Descriptions.end(); ++it) {
    
        if (it->second.visualizerWidget == visualizatorWidget) {
            collection.push_back(it->first);
        }
    }*/

    // TODO : raw ptr tez nie dziala, bo nie jest kompatybilny... 
    /*core::HierarchyHelperPtr helper  = analysisModel->getHelper(desc);
    if (helper && helper->getParent()) {
        collection.push_back(helper->getParent());
        summary->display(collection);
    }*/
}
