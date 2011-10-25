#include "hmmPCH.h"
#include "FilterCommand.h"
#include "HmmMainWindow.h"
#include "TreeBuilder.h"


using namespace core;

SimpleFilterCommand::SimpleFilterCommand( DataFilterPtr dataFilter ) :
    dataFilter(dataFilter)
{

}


//QTreeWidgetItem* SimpleFilterCommand::createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
//{
//    return TreeBuilder::createTree(rootItemName, sessions, dataFilter);
//}

//QTreeWidgetItem* SimpleFilterCommand::createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions)
QWidget* SimpleFilterCommand::getConfigurationWidget(QWidget* parent)
{
    QHBoxLayout* layout = new QHBoxLayout();
    QWidget* widget = new QWidget(parent);
    widget->setLayout(layout);
    QLabel* label = new QLabel(widget);
    label->setText("Configuration widget");
    layout->addWidget(label);
    QDoubleSpinBox* spin = new QDoubleSpinBox(widget);
    layout->addWidget(spin);
    return widget;
}

/*QTreeWidgetItem* SimpleFilterCommand::createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions, DataFilterPtr dataFilter)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);
    for (int i = sessions.size() - 1; i >= 0; --i) {
        SessionConstPtr session = dataFilter ? dataFilter->filterData(sessions[i]) : sessions[i];
        
        QIcon icon(core::getResourceString("icons/charts.png"));
        QIcon icon3D(core::getResourceString("icons/3d.png"));
        QIcon iconVideo(core::getResourceString("icons/video.png"));*/



