#include "SummaryWindow.h"
#include "TreeItemHelper.h"
#include "HmmMainWindow.h"
#include "AnalisisWidget.h"
#include "VisualizerWidget.h"
#include "TreeItemHelper.h"
#include <plugins/newCommunication/IPatient.h>

void SummaryWindow::initialize()
{
    QWidget* parent = hmm->getAnalisis()->SummaryWindowWidget;
    parent->setLayout(new QVBoxLayout());
    parent->layout()->setContentsMargins(0, 0, 0, 0);
    parent->layout()->setMargin(0);
    tree = new QTreeWidget();
    tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tree->setHeaderHidden(true);
    parent->layout()->addWidget(tree);
}

void SummaryWindow::display( const std::vector<TreeItemHelper*>& helpers )
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

        for (auto itHelper = helpers.begin(); itHelper != helpers.end(); itHelper++) {
            
            NewMultiserieHelper* multiserieHelper = dynamic_cast<NewMultiserieHelper*>(*itHelper);
            if (multiserieHelper) {
                const auto& desc = multiserieHelper->getChartWithDescriptions();
                for (auto it = desc.begin(); it != desc.end(); it++) {
                    auto motion = it->motion;
                    if (motion) {
                        QString text = "<b>" + QString::fromStdString(it->wrapper->getName()) + "</b>\n" + createDescription(it->motion);
                        addItem(text, root);
                    }
                }
            } else {
                auto motion = (*itHelper)->getMotion();
                QString text = motion ? createDescription(motion) : QString();
                text = (*itHelper)->text(0) + "\n" + text;
                //TreeWrappedItemHelper* wrapped = dynamic_cast<TreeWrappedItemHelper*>(*itHelper);
                //if (wrapped) {
                //    text = QString::fromStdString(wrapped->getWrapper()->getName()) + "\n" + text;
                //} 
                addItem(text, root);

            }
            
            
        }

        root->setExpanded(true);
    }
}

QString SummaryWindow::createDescription( PluginSubject::MotionConstPtr motion) const
{
    QString text;
    PluginSubject::SessionConstPtr session = motion->getSession();
    PluginSubject::SubjectConstPtr subject = session->getSubject();
    
    text += QObject::tr("Motion: ") + QString::fromStdString(motion->getLocalName()) + "\n";
    text += QObject::tr("Session: ") + QString::fromStdString(session->getLocalName()) + "\n";
    text += QObject::tr("Subject: ") + QString::fromStdString(subject->getName()) + "\n";
    if (session->hasObjectOfType(typeid(communication::IPatient))) {
        communication::PatientConstPtr patient = session->getWrapperOfType(typeid(communication::IPatient))->get();
        text += QObject::tr("Patient: ") + QString::fromStdString(patient->getName()) + " " + QString::fromStdString(patient->getSurname()) + "\n";
    }

    if (session->hasObjectOfType(typeid(communication::AntropometricData))) {
        core::shared_ptr<communication::AntropometricData> antro = session->getWrapperOfType(typeid(communication::AntropometricData))->get();
        text += QObject::tr("Weight: ") + QString("%1 ").arg(antro->bodyMass.first) + QString::fromStdString(antro->bodyMass.second) + "\n";
    }
    return text;
}

void SummaryWindow::addItem( QString text, QTreeWidgetItem* root )
{
    QTreeWidgetItem* itm = new QTreeWidgetItem();
    itm->setText(0, text);
    root->addChild(itm);
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
