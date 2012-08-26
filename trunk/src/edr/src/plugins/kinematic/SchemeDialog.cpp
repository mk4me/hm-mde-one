#include "PCH.h"
#include <QtGui/QTableView>
#include "SchemeDialog.h"
#include "SchemeDrawerContainer.h"
#include "GlLineSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"


SchemeDialog::SchemeDialog( QWidget* parent) :
	QDialog(parent)
{
	Ui::TrajectoriesDialog::setupUi(this);
    Ui::TrajectoriesDialog::propertiesBox->setVisible(false);
	QIcon icon( QString::fromUtf8(":/resources/icons/trajectory.png") );
	QTreeWidgetItem* item = tree->headerItem();
    tree->setColumnCount(3);
	item->setIcon(0, icon);
	item->setText(0, "");
    item->setText(2, tr("Color"));
}

void SchemeDialog::setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(1, rootName);
    tree->addTopLevelItem(root);

    QPushButton* globalColorButton = new QPushButton();
    globalColorButton->setFixedSize(16, 16);
    connect(globalColorButton, SIGNAL(clicked(bool)), this, SLOT(globalColorClicked()));
    tree->setItemWidget(root, 2, globalColorButton);
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
            for (unsigned int i = 0; i < mrks.size();  ++i) {
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
    int columnCount = tree->columnCount();
    for (int c = 0; c < columnCount; ++c) {
        if (tree->itemWidget(item, c) == object) {
            return item;
        }
    }

    for (int c = 0; c < item->childCount(); ++c) {
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
	for (int i = 0; i < count; ++i) {
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

void SchemeDialog::globalColorClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QTreeWidgetItem* item = getItemWhichContains(button);
        VisualizationSchemePtr scheme = core::const_pointer_cast<VisualizationScheme>(item2Root[item]->getVisualiztionScheme());
        if (scheme) {
            QColor color(255, 255, 255, 80);
            color = QColorDialog::getColor ( color, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);
            scheme->setGlobalStatesColor(transformColor(color));
            item2Root[item]->update();
        }
    }
}
