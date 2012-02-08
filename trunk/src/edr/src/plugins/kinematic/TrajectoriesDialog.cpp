#include "PCH.h"
#include <QtGui/QTableView>
#include "TrajectoriesDialog.h"
#include "SchemeDrawerContainer.h"

const int columnCount = 2;

TrajectoriesDialog::TrajectoriesDialog( QWidget* parent) :
	QDialog(parent)
{
	Ui::TrajectoriesDialog::setupUi(this);
	QIcon icon( QString::fromUtf8(":/resources/icons/trajectory.png") );
	QTreeWidgetItem* item = tree->headerItem();
	item->setIcon(0, icon);
	item->setText(0, "");
    
	connect(Ui::TrajectoriesDialog::colorButton, SIGNAL(clicked()), this, SLOT(colorClicked()));
	connect(thicknessSpin, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));
	connect(startTimeSpin, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
	connect(endTimeSpin,   SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
	connect(startSlider,   SIGNAL(valueChanged(int)), this, SLOT(startSliderChanged(int)));
	connect(endSlider,     SIGNAL(valueChanged(int)), this, SLOT(endSliderChanged(int)));
	connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		    this, SLOT(treeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

void TrajectoriesDialog::setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootName);
    tree->addTopLevelItem(root);
    QCheckBox* rootCheck = new QCheckBox(tree);
    rootCheck->setChecked(true);
    tree->setItemWidget(root, 0, rootCheck);
    connect(rootCheck, SIGNAL(clicked(bool)), this, SLOT(rootVisibilityChanged(bool)));
    item2Root[root] = drawer;
    
    SchemeDrawerContainerPtr container = core::dynamic_pointer_cast<SchemeDrawerContainer>(drawer);
    if (container) {
        BOOST_FOREACH (OsgSchemeDrawerPtr child, container->getDrawers()) {
            TrajectoryDrawerPtr trajectories = core::dynamic_pointer_cast<TrajectoryDrawer>(child);
            if (trajectories) {
                MarkerCollectionConstPtr markers = trajectories->getMarkers();
                int count = markers->getNumChannels();
                if (count) {
                    for (int i = 0; i < count; i++) {
                        QTreeWidgetItem* item = new QTreeWidgetItem();

                        item->setText(1, markers->getChannel(i)->getName().c_str());

                        item->setFlags(item->flags() | Qt::ItemIsSelectable);
                        item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
                        root->addChild(item);

                        QCheckBox* check1 = new QCheckBox(tree);
                        tree->setItemWidget(item, 0, check1);
                        connect(check1, SIGNAL(clicked(bool)), this, SLOT(visibilityChanged(bool)));
                        item2Trajectories[item] = trajectories;
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
                }
            }
        }
       
    }
    


    
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


QTreeWidgetItem* getItemWhichContainsRecurr(QTreeWidget* tree, QTreeWidgetItem* item, QObject* object )
{
    for (int c = 0; c < columnCount; c++) {
        if (tree->itemWidget(item, c) == object) {
            return item;
        }
    }

    for (int c = 0; c < item->childCount(); c++) {
        QTreeWidgetItem* child = getItemWhichContainsRecurr(tree, item->child(c), object);
        if (child) {
            return child;
        }
    }

    return nullptr;
}

QTreeWidgetItem* TrajectoriesDialog::getItemWhichContains( QObject* object ) const
{
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
        QTreeWidgetItem* item = getItemWhichContainsRecurr(tree, tree->topLevelItem(i), object);
        if (item) {
            return item;
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
		currentTrajectories->setVisible(item->text(1).toStdString(), visible);

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
	QColor color(255, 255, 255, 80);// = transformColor(trajectories->getColor(name));
	color = QColorDialog::getColor ( color, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);

    for (int j = 0; j < tree->topLevelItemCount(); j++) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; i++) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                std::string name = item->text(1).toStdString();
                currentTrajectories->setColor(name, transformColor(color));
                setButtonColor(colorButton, color);
            }
        }
    }
}

void TrajectoriesDialog::widthChanged( double width )
{
    for (int j = 0; j < tree->topLevelItemCount(); j++) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; i++) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                std::string name = item->text(1).toStdString();
                currentTrajectories->setLineWidth(name, static_cast<float>(width));
            }
        }
    }
}

void TrajectoriesDialog::startTimeChanged( double time )
{
    for (int j = 0; j < tree->topLevelItemCount(); j++) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; i++) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                std::string name = item->text(1).toStdString();
                const std::pair<float, float>& times = currentTrajectories->getTimes(name);
                blockAllSignals(true);
                startTimeSpin->setValue(time);
                startSlider->setValue(time * 100);
                blockAllSignals(false);
                currentTrajectories->setTimes(name, std::make_pair(static_cast<float>(time), times.second));
            }
        }
    }
}

void TrajectoriesDialog::endTimeChanged( double time )
{
    for (int j = 0; j < tree->topLevelItemCount(); j++) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; i++) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                std::string name = item->text(1).toStdString();
                blockAllSignals(true);
                endTimeSpin->setValue(time);
                endSlider->setValue(time * 100);
                blockAllSignals(false);
                const std::pair<float, float>& times = currentTrajectories->getTimes(name);
                currentTrajectories->setTimes(name, std::make_pair(times.first, static_cast<float>(time)));
            }
        }
    }
}

void TrajectoriesDialog::treeItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	std::string name = current->text(1).toStdString();
    currentTrajectories = item2Trajectories[current];
    // w drzewie moga istniec elementy, nie odnoszace sie bezposrednio do trajektorii.
    if (!currentTrajectories) {
        return;         // <----------   return !
    }

	QColor color = transformColor(currentTrajectories->getColor(name));
	setButtonColor(colorButton, color);
	blockAllSignals(true);
	const std::pair<float, float>& times = currentTrajectories->getTimes(name);
	startTimeSpin->setValue(times.first);
	endTimeSpin->setValue(times.second);
	startSlider->setValue(times.first * 100);
	endSlider->setValue(times.second * 100);
	float thickness = currentTrajectories->getLineWidth(name);
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

void TrajectoriesDialog::rootVisibilityChanged( bool visible )
{
    QCheckBox* box = qobject_cast<QCheckBox*>(sender());
    if (box) {
        QTreeWidgetItem* item = getItemWhichContains(box);
        item2Root[item]->setVisible(visible);
    }
}
