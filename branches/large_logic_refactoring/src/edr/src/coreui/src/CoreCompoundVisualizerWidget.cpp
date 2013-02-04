#include "CoreUiPCH.h"
#include <coreui/CoreCompoundVisualizerWidget.h>
#include <coreui/CoreVisualizerWidget.h>
#include <QtGui/QHBoxLayout>

using namespace coreUI;
using namespace core;

Q_DECLARE_METATYPE(CoreVisualizerWidget*);

CoreCompoundVisualizerWidget::CoreCompoundVisualizerWidget(CoreCompoundVisualizerWidget & visualizer)
{

}

CoreCompoundVisualizerWidget::CoreCompoundVisualizerWidget() : visualizerSwitch(new QComboBox)
{
	visualizerSwitch->setParent(this);
	setLayout(new QHBoxLayout);
	visualizerSwitch->setEditText(tr("No visualizer"));
}

CoreCompoundVisualizerWidget::~CoreCompoundVisualizerWidget()
{

}

void CoreCompoundVisualizerWidget::addVisualizer(VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);
	if(visualizerSwitch->count() == 0){
		visualizerWidget->setVisible(true);
	}else{
		visualizerWidget->setVisible(false);
	}

	layout()->addWidget(visualizerWidget);

	QVariant data;
	data.setValue(visualizerWidget);
	visualizerSwitch->addItem(visualizer->getIcon(), visualizerName, data);
}

void CoreCompoundVisualizerWidget::addVisualizer(const QIcon & icon, VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);
	if(visualizerSwitch->count() == 0){
		visualizerWidget->setVisible(true);
	}else{
		visualizerWidget->setVisible(false);
	}

	layout()->addWidget(visualizerWidget);

	QVariant data;
	data.setValue(visualizerWidget);
	visualizerSwitch->addItem(icon, visualizerName, data);
}

int CoreCompoundVisualizerWidget::count() const
{
	return visualizerSwitch->count();
}

int CoreCompoundVisualizerWidget::currentIndex() const
{
	return visualizerSwitch->currentIndex();
}

VisualizerPtr CoreCompoundVisualizerWidget::currentVisualizer() const
{
	CoreVisualizerWidget * currentVisWidget = currentVisualizerWidget();

	if(currentVisWidget == nullptr){
		return VisualizerPtr();
	}

	return currentVisWidget->getVisualizer();
}

CoreVisualizerWidget * CoreCompoundVisualizerWidget::currentVisualizerWidget() const
{
	if(visualizerSwitch->count() == 0){
		return nullptr;
	}

	return visualizerSwitch->itemData(visualizerSwitch->currentIndex()).value<CoreVisualizerWidget*>();
}

bool CoreCompoundVisualizerWidget::nameDuplicatesEnabled() const
{
	return visualizerSwitch->duplicatesEnabled();
}

int CoreCompoundVisualizerWidget::findVisualizer(core::VisualizerPtr visualizer) const
{
	int ret = -1;

	for(int i = 0; i < visualizerSwitch->count(); ++i){
		if(visualizerSwitch->itemData(i).value<CoreVisualizerWidget*>()->getVisualizer() == visualizer){
			ret = i;
			break;
		}
	}

	return ret;
}

int CoreCompoundVisualizerWidget::findVisualizer(const QString & visualizerName) const
{
	return visualizerSwitch->findText(visualizerName);
}

void CoreCompoundVisualizerWidget::insertVisualizer(int index, core::VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);
	if(visualizerSwitch->count() == 0){
		visualizerWidget->setVisible(true);
	}else{
		visualizerWidget->setVisible(false);
	}

	layout()->addWidget(visualizerWidget);

	QVariant data;
	data.setValue(visualizerWidget);
	visualizerSwitch->insertItem(index, visualizer->getIcon(), visualizerName, data);
}

void CoreCompoundVisualizerWidget::insertVisualizer(int index, const QIcon & icon, core::VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);
	if(visualizerSwitch->count() == 0){
		visualizerWidget->setVisible(true);
	}else{
		visualizerWidget->setVisible(false);
	}

	layout()->addWidget(visualizerWidget);

	QVariant data;
	data.setValue(visualizerWidget);
	visualizerSwitch->insertItem(index, icon, visualizerName, data);
}

QComboBox::InsertPolicy CoreCompoundVisualizerWidget::insertPolicy() const
{
	return visualizerSwitch->insertPolicy();
}

void CoreCompoundVisualizerWidget::insertSeparator(int index)
{
	visualizerSwitch->insertSeparator(index);
}

QIcon CoreCompoundVisualizerWidget::visualizerIcon(int index) const
{
	return visualizerSwitch->itemIcon(index);
}

QString CoreCompoundVisualizerWidget::visualizerText(int index) const
{
	return visualizerSwitch->itemText(index);
}

int CoreCompoundVisualizerWidget::maxCount() const
{
	return visualizerSwitch->maxCount();
}

int CoreCompoundVisualizerWidget::maxVisibleItems() const
{
	return visualizerSwitch->maxVisibleItems();
}

int CoreCompoundVisualizerWidget::minimumContentsLength() const
{
	return visualizerSwitch->minimumContentsLength();
}

void CoreCompoundVisualizerWidget::removeVisualizer(int index)
{
	CoreVisualizerWidget * visWidget = visualizerSwitch->itemData(index).value<CoreVisualizerWidget *>();

	if(index == currentIndex()){
		visWidget->setVisible(false);
		visualizerSwitch->removeItem(index);
		innerShowCurrentIndex(currentIndex());
	}

	layout()->removeWidget(visWidget);
	delete visWidget;
}

void CoreCompoundVisualizerWidget::setNameDuplicatesEnabled(bool enable)
{
	visualizerSwitch->setDuplicatesEnabled(enable);
}

void CoreCompoundVisualizerWidget::setIconSize(const QSize & size)
{
	visualizerSwitch->setIconSize(size);
}

void CoreCompoundVisualizerWidget::setInsertPolicy(QComboBox::InsertPolicy policy)
{
	visualizerSwitch->setInsertPolicy(policy);
}

void CoreCompoundVisualizerWidget::setVisualizerIcon(int index, const QIcon & icon)
{
	visualizerSwitch->setItemIcon(index, icon);
}

void CoreCompoundVisualizerWidget::setVisualizerText(int index, const QString & text)
{
	visualizerSwitch->setItemText(index, text);
}

void CoreCompoundVisualizerWidget::setMaxCount(int max)
{
	visualizerSwitch->setMaxCount(max);
}

void CoreCompoundVisualizerWidget::setMaxVisibleVisualizers(int maxItems)
{
	visualizerSwitch->setMaxVisibleItems(maxItems);
}

void CoreCompoundVisualizerWidget::setMinimumContentsLength(int characters)
{
	visualizerSwitch->setMinimumContentsLength(characters);
}

void CoreCompoundVisualizerWidget::setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy policy)
{
	visualizerSwitch->setSizeAdjustPolicy(policy);
}

QComboBox::SizeAdjustPolicy CoreCompoundVisualizerWidget::sizeAdjustPolicy() const
{
	return visualizerSwitch->sizeAdjustPolicy();
}

void CoreCompoundVisualizerWidget::clear()
{
	visualizerSwitch->clear();
}

void CoreCompoundVisualizerWidget::setCurrentIndex(int index)
{
	CoreVisualizerWidget * currentVisWidget = currentVisualizerWidget();

	if(currentVisWidget != nullptr){
		currentVisWidget->setVisible(false);
	}

	visualizerSwitch->setCurrentIndex(index);

	innerShowCurrentIndex(index);
}

void CoreCompoundVisualizerWidget::innerShowCurrentIndex(int idx)
{
	if(idx == -1){
		visualizerSwitch->setEditText(tr("No visualizers"));
	}else{
		visualizerSwitch->itemData(idx).value<CoreVisualizerWidget *>()->setVisible(true);	
	}

	emit currentIndexChanged(idx);
}