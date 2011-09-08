#include "PCH.h"
#include "TrajectoriesDialog.h"
#include <QtGui/QTableView>

const int columnCount = 4;

TrajectoriesDialog::TrajectoriesDialog( QWidget* parent, TrajectoryDrawerPtr trajectoryDrawer ) :
	QDialog(parent),
	trajectories(trajectoryDrawer)
{
	QGridLayout* grid = new QGridLayout(this);
	tree = new QTreeWidget(this);
	tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	grid->addWidget(tree);
	setLayout(grid);
}

void TrajectoriesDialog::setMarkers( MarkerCollectionConstPtr markers )
{
	QStringList headers;
	headers << "Marker" << "Visible" << "Thickness" << "Color";
	UTILS_ASSERT(columnCount == headers.size());
	tree->setHeaderLabels(headers);

	int count = markers->getNumChannels();

	tree->setSelectionMode(QAbstractItemView::MultiSelection);

	
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		
		item->setText(0, markers->getChannel(i)->getName().c_str());
		
		item->setFlags(item->flags() | Qt::ItemIsSelectable);
		tree->addTopLevelItem(item);

		QCheckBox* check1 = new QCheckBox(tree);
		tree->setItemWidget(item, 1, check1);

		QDoubleSpinBox* spin = new QDoubleSpinBox(tree);
		spin->setValue(1.0);
		spin->setSingleStep(0.1);
		tree->setItemWidget(item, 2, spin);

		QPushButton* button = new QPushButton(tree);
		setButtonColor(button, QColor(255,255,255,128));
		tree->setItemWidget(item, 3, button);

		connect(button, SIGNAL(clicked()), this, SLOT(colorClicked()));
		connect(check1, SIGNAL(clicked(bool)), this, SLOT(visibilityChanged(bool)));
		connect(spin, SIGNAL(valueChanged ( double  )), this, SLOT(widthChanged(double)));
	}

	this->setMinimumSize(450, 600);
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
		trajectories->setVisible(item->text(0).toStdString(), visible);
	}
}

void TrajectoriesDialog::colorClicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if (button) {
		QTreeWidgetItem* item = getItemWhichContains(button);
		if (item) {
			std::string name = item->text(0).toStdString();
			QColor color = transformColor(trajectories->getColor(name));
			color = QColorDialog::getColor ( color, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);
			trajectories->setColor(name, transformColor(color));
			setButtonColor(button, color);
		}
	}
}

void TrajectoriesDialog::widthChanged( double width )
{
	QDoubleSpinBox* spin = qobject_cast<QDoubleSpinBox*>(sender());
	if (spin) {
		QTreeWidgetItem* item = getItemWhichContains(spin);
		if (item) {
			std::string name = item->text(0).toStdString();
			trajectories->setLineWidth(name, static_cast<float>(width));
		}
	}
}
