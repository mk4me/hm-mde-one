#include "PCH.h"
#include "TrajectoriesDialog.h"
#include <QtGui/QTableView>

const int columnCount = 2;

TrajectoriesDialog::TrajectoriesDialog( QWidget* parent, TrajectoryDrawerPtr trajectoryDrawer ) :
	QDialog(parent),
	trajectories(trajectoryDrawer)
{
	Ui::TrajectoriesDialog::setupUi(this);
	QIcon icon( core::getResourceString("icons/trajectory.png") );
	QTreeWidgetItem* item = tree->headerItem();
	item->setIcon(0, icon);
	item->setText(0, "");
}

void TrajectoriesDialog::setMarkers( MarkerCollectionConstPtr markers )
{
	int count = markers->getNumChannels();

	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		
		item->setText(1, markers->getChannel(i)->getName().c_str());
		
		item->setFlags(item->flags() | Qt::ItemIsSelectable);
		item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
		tree->addTopLevelItem(item);

		QCheckBox* check1 = new QCheckBox(tree);
		tree->setItemWidget(item, 0, check1);
		connect(check1, SIGNAL(clicked(bool)), this, SLOT(visibilityChanged(bool)));
	}

	int maxVal = static_cast<int> (100 * markers->getLength()) - 1;
	startSlider->setMinimum(0);
	startSlider->setMaximum(maxVal-1);
	endSlider->setMinimum(1);
	endSlider->setMaximum(maxVal);
	endSlider->setValue(maxVal);
	startTimeSpin->setMinimum(0);
	startTimeSpin->setMaximum(markers->getLength() - 0.001f);
	endTimeSpin->setMinimum(1);
	endTimeSpin->setMaximum(markers->getLength());
	endTimeSpin->setValue(markers->getLength());


	connect(Ui::TrajectoriesDialog::colorButton, SIGNAL(clicked()), this, SLOT(colorClicked()));
	connect(thicknessSpin, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));
	connect(startTimeSpin, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
	connect(endTimeSpin,   SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
	connect(startSlider,   SIGNAL(valueChanged(int)), this, SLOT(startSliderChanged(int)));
	connect(endSlider,     SIGNAL(valueChanged(int)), this, SLOT(endSliderChanged(int)));
	connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		    this, SLOT(treeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

void TrajectoriesDialog::onTreeItemClicked( QTreeWidgetItem* item, int column )
{
	LOG_WARNING("TST");
}



QColor TrajectoriesDialog::transformColor( const osg::Vec4& color ) const
{
	return QColor(
		static_cast<int>(255 * color[0]),
		static_cast<int>(255 * color[1]),
		static_cast<int>(255 * color[2]),
		static_cast<int>(255 * color[3])
		);
}

osg::Vec4 TrajectoriesDialog::transformColor( const QColor& color ) const
{
	return osg::Vec4 (
		static_cast<float>(color.red() / 255.0f),
		static_cast<float>(color.green() / 255.0f),
		static_cast<float>(color.blue() / 255.0f),
		static_cast<float>(color.alpha() / 255.0f)
		);

}

QTreeWidgetItem* TrajectoriesDialog::getItemWhichContains( QObject* object ) const
{
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = tree->topLevelItem(i);
		for (int c = 0; c < columnCount; c++) {
			if (tree->itemWidget(item, c) == object) {
				return item;
			}
		}
	}
	throw std::runtime_error("Tree item not found");
}

void TrajectoriesDialog::setButtonColor( QPushButton* button, const QColor& color )
{
	QString style = QString("QPushButton { background-color: rgb( %1, %2, %3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	button->setStyleSheet(style);
}

void TrajectoriesDialog::visibilityChanged( bool visible )
{
	QCheckBox* box = qobject_cast<QCheckBox*>(sender());
	if (box) {
		QTreeWidgetItem* item = getItemWhichContains(box);
		item->setSelected(true);
		trajectories->setVisible(item->text(1).toStdString(), visible);

		int count = tree->topLevelItemCount();
		for (int i = 0; i < count; i++) {
			QTreeWidgetItem* itemToDeselect = tree->topLevelItem(i);
			if (itemToDeselect != item) {
				itemToDeselect->setSelected(false);
			}
		}

		treeItemChanged(item, nullptr);
	}


}

void TrajectoriesDialog::colorClicked()
{
	QColor color;// = transformColor(trajectories->getColor(name));
	color = QColorDialog::getColor ( color, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = tree->topLevelItem(i);
		if (item && item->isSelected()) {
			std::string name = item->text(1).toStdString();
			trajectories->setColor(name, transformColor(color));
			setButtonColor(colorButton, color);
		}
	}
}

void TrajectoriesDialog::widthChanged( double width )
{
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = tree->topLevelItem(i);
		if (item && item->isSelected()) {
			std::string name = item->text(1).toStdString();
			trajectories->setLineWidth(name, static_cast<float>(width));
		}
	}
}

void TrajectoriesDialog::startTimeChanged( double time )
{
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = tree->topLevelItem(i);
		if (item && item->isSelected()) {
			std::string name = item->text(1).toStdString();
			const std::pair<float, float>& times = trajectories->getTimes(name);
			blockAllSignals(true);
			startTimeSpin->setValue(time);
			startSlider->setValue(time * 100);
			blockAllSignals(false);
			trajectories->setTimes(name, std::make_pair(static_cast<float>(time), times.second));
		}
	}
}

void TrajectoriesDialog::endTimeChanged( double time )
{
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = tree->topLevelItem(i);
		if (item && item->isSelected()) {
			std::string name = item->text(1).toStdString();
			blockAllSignals(true);
			endTimeSpin->setValue(time);
			endSlider->setValue(time * 100);
			blockAllSignals(false);
			const std::pair<float, float>& times = trajectories->getTimes(name);
			trajectories->setTimes(name, std::make_pair(times.first, static_cast<float>(time)));
		}
	}
}

void TrajectoriesDialog::treeItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	std::string name = current->text(1).toStdString();

	QColor color = transformColor(trajectories->getColor(name));
	setButtonColor(colorButton, color);
	blockAllSignals(true);
	const std::pair<float, float>& times = trajectories->getTimes(name);
	startTimeSpin->setValue(times.first);
	endTimeSpin->setValue(times.second);
	startSlider->setValue(times.first * 100);
	endSlider->setValue(times.second * 100);
	float thickness = trajectories->getLineWidth(name);
	thicknessSpin->setValue(thickness);
	blockAllSignals(false);
}

void TrajectoriesDialog::startSliderChanged(int value)
{
	if (endSlider->value() > value) {
		startTimeChanged(value * 0.01f);
	} else {
		startSlider->setValue(endSlider->value() - 1);
	}
}

void TrajectoriesDialog::endSliderChanged(int value)
{
	if (value > startSlider->value()) {
		endTimeChanged(value * 0.01f);
	} else {
		endSlider->setValue(startSlider->value() + 1);
	}
}

void TrajectoriesDialog::blockAllSignals( bool val )
{
	this->blockSignals(val);
	startSlider->blockSignals(val);
	startTimeSpin->blockSignals(val);
	endTimeSpin->blockSignals(val);
	endSlider->blockSignals(val);
	thicknessSpin->blockSignals(val);
	
}