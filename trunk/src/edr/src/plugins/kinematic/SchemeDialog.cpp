#include "PCH.h"
#include <QtGui/QTableView>
#include "SchemeDialog.h"
#include "SchemeDrawerContainer.h"
#include "GlLineSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"


const int columnCount = 2;

SchemeDialog::SchemeDialog( QWidget* parent) :
	QDialog(parent)
{
	Ui::TrajectoriesDialog::setupUi(this);
    Ui::TrajectoriesDialog::propertiesBox->setVisible(false);
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

void SchemeDialog::setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(1, rootName);
    tree->addTopLevelItem(root);
    QCheckBox* rootCheck = new QCheckBox(tree);
    rootCheck->setChecked(true);
    tree->setItemWidget(root, 0, rootCheck);
    connect(rootCheck, SIGNAL(clicked(bool)), this, SLOT(rootVisibilityChanged(bool)));
    item2Root[root] = drawer;
    
    SchemeDrawerContainerPtr container = core::dynamic_pointer_cast<SchemeDrawerContainer>(drawer);
    if (container) {
        GlLineSchemeDrawerPtr lines;
        GlPointSchemeDrawerPtr points;
        BOOST_FOREACH (OsgSchemeDrawerPtr child, container->getDrawers()) {
            GlLineSchemeDrawerPtr l = core::dynamic_pointer_cast<GlLineSchemeDrawer>(child);
            if (l) {
                lines = l;
            }

            GlPointSchemeDrawerPtr p = core::dynamic_pointer_cast<GlPointSchemeDrawer>(child);
            if (p) {
                points = p;
            }
            
        }

        if (points && lines) {
            VisualizationSchemeConstPtr scheme = lines->getVisualiztionScheme();
            const std::vector<VisualizationScheme::Connection>& cons = scheme->getConnections();
            const std::vector<VisualizationScheme::State>& mrks = scheme->getStates();
            for (int i = 0; i < mrks.size();  i++) {
                QTreeWidgetItem* item = new QTreeWidgetItem();

                item->setText(1, mrks[i].name.c_str());
                root->addChild(item);

                QCheckBox* check1 = new QCheckBox(tree);
                check1->setChecked(true);
                tree->setItemWidget(item, 0, check1);
                connect(check1, SIGNAL(clicked(bool)), this, SLOT(visibilityChanged(bool)));
                item2Drawer[item] = boost::make_tuple(points, lines, i);
            }
        }
            //TrajectoryDrawerPtr trajectories = core::dynamic_pointer_cast<TrajectoryDrawer>(child);
            //if (trajectories) {
            //    MarkerCollectionConstPtr markers = trajectories->getMarkers();
            //    int count = markers->getNumChannels();
            //    if (count) {
            //        for (int i = 0; i < count; i++) {
            //            QTreeWidgetItem* item = new QTreeWidgetItem();
            //
            //            item->setText(1, markers->getChannel(i)->getName().c_str());
            //
            //            item->setFlags(item->flags() | Qt::ItemIsSelectable);
            //            item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
            //            root->addChild(item);
            //
            //            QCheckBox* check1 = new QCheckBox(tree);
            //            tree->setItemWidget(item, 0, check1);
            //            connect(check1, SIGNAL(clicked(bool)), this, SLOT(visibilityChanged(bool)));
            //            item2Trajectories[item] = trajectories;
            //        }
            //    
            //        int maxVal = static_cast<int> (100 * markers->getLength()) - 1;
            //        startSlider->setMinimum(0);
            //        startSlider->setMaximum(maxVal-1);
            //        endSlider->setMinimum(1);
            //        endSlider->setMaximum(maxVal);
            //        endSlider->setValue(maxVal);
            //        startTimeSpin->setMinimum(0);
            //        startTimeSpin->setMaximum(markers->getLength() - 0.001f);
            //        endTimeSpin->setMinimum(1);
            //        endTimeSpin->setMaximum(markers->getLength());
            //        endTimeSpin->setValue(markers->getLength());
            //    }
            //}
        }
       
    }
    





QColor SchemeDialog::transformColor( const osg::Vec4& color ) const
{
	return QColor(
		static_cast<int>(255 * color[0]),
		static_cast<int>(255 * color[1]),
		static_cast<int>(255 * color[2]),
		static_cast<int>(255 * color[3])
		);
}

osg::Vec4 SchemeDialog::transformColor( const QColor& color ) const
{
	return osg::Vec4 (
		static_cast<float>(color.red() / 255.0f),
		static_cast<float>(color.green() / 255.0f),
		static_cast<float>(color.blue() / 255.0f),
		static_cast<float>(color.alpha() / 255.0f)
		);

}


QTreeWidgetItem* _getItemWhichContainsRecurr(QTreeWidget* tree, QTreeWidgetItem* item, QObject* object )
{
    for (int c = 0; c < columnCount; c++) {
        if (tree->itemWidget(item, c) == object) {
            return item;
        }
    }

    for (int c = 0; c < item->childCount(); c++) {
        QTreeWidgetItem* child = _getItemWhichContainsRecurr(tree, item->child(c), object);
        if (child) {
            return child;
        }
    }

    return nullptr;
}

QTreeWidgetItem* SchemeDialog::getItemWhichContains( QObject* object ) const
{
	int count = tree->topLevelItemCount();
	for (int i = 0; i < count; i++) {
        QTreeWidgetItem* item = _getItemWhichContainsRecurr(tree, tree->topLevelItem(i), object);
        if (item) {
            return item;
        }
		
	}
	throw std::runtime_error("Tree item not found");
}

void SchemeDialog::setButtonColor( QPushButton* button, const QColor& color )
{
	QString style = QString("QPushButton { background-color: rgb( %1, %2, %3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	button->setStyleSheet(style);
}

void SchemeDialog::visibilityChanged( bool visible )
{
	QCheckBox* box = qobject_cast<QCheckBox*>(sender());
	if (box) {
		QTreeWidgetItem* current = getItemWhichContains(box);
        //std::string name = current->text(1).toStdString();
        auto it = item2Drawer.find(current);
        if (it != item2Drawer.end()) {
            auto t = it->second;
            GlPointSchemeDrawerPtr points = t.get<0>();
            GlLineSchemeDrawerPtr lines = t.get<1>();
            int i = t.get<2>();
            VisualizationSchemeConstPtr scheme = points->getVisualiztionScheme();
            typedef VisualizationScheme::State JointState;
            typedef VisualizationScheme::Connection Connection;
            std::vector<JointState> & mrks = const_cast<std::vector<JointState>&>(scheme->getStates());
            mrks[i].visible = box->isChecked();
            points->update();
            lines->update();
        }
		//item->setSelected(true);
		//currentTrajectories->setVisible(item->text(1).toStdString(), visible);
        //
		//int count = tree->topLevelItemCount();
		//for (int i = 0; i < count; i++) {
		//	QTreeWidgetItem* itemToDeselect = tree->topLevelItem(i);
		//	if (itemToDeselect != item) {
		//		itemToDeselect->setSelected(false);
		//	}
		//}
        //
		//treeItemChanged(item, nullptr);
	}


}

void SchemeDialog::colorClicked()
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

void SchemeDialog::widthChanged( double width )
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

void SchemeDialog::startTimeChanged( double time )
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

void SchemeDialog::endTimeChanged( double time )
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

void SchemeDialog::treeItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	/*std::string name = current->text(1).toStdString();
    auto it = item2Drawer.find(current);
    if (it != item2Drawer.end()) {
        auto t = it->second;
        GlPointSchemeDrawerPtr points = t.get<0>();
        GlLineSchemeDrawerPtr lines = t.get<1>();
        int i = t.get<2>();
        SkeletalVisualizationSchemeConstPtr scheme = points->getVisualiztionScheme();
        typedef SkeletalVisualizationScheme::JointState JointState;
        std::vector<JointState> & mrks = const_cast<std::vector<JointState>&>(scheme->getMarkersStates());
        mrks[i].color = osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f);
    }*/
}

void SchemeDialog::startSliderChanged(int value)
{
	if (endSlider->value() > value) {
		startTimeChanged(value * 0.01f);
	} else {
		startSlider->setValue(endSlider->value() - 1);
	}
}

void SchemeDialog::endSliderChanged(int value)
{
	if (value > startSlider->value()) {
		endTimeChanged(value * 0.01f);
	} else {
		endSlider->setValue(startSlider->value() + 1);
	}
}

void SchemeDialog::blockAllSignals( bool val )
{
	this->blockSignals(val);
	startSlider->blockSignals(val);
	startTimeSpin->blockSignals(val);
	endTimeSpin->blockSignals(val);
	endSlider->blockSignals(val);
	thicknessSpin->blockSignals(val);
	
}

void SchemeDialog::rootVisibilityChanged( bool visible )
{
    QCheckBox* box = qobject_cast<QCheckBox*>(sender());
    if (box) {
        QTreeWidgetItem* item = getItemWhichContains(box);
        item2Root[item]->setVisible(visible);
    }
}
