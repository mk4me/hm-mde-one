/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   12:49
	filename: 	TrajectoriesDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TRAJECTORIES_DIALOG__TRAJECTORIESDIALOG_H__
#define HEADER_GUARD_TRAJECTORIES_DIALOG__TRAJECTORIESDIALOG_H__


#include <core/IVisualizer.h>
#include <osg/Geode>
#include <timelinelib/IChannel.h>
#include "KinematicVisualizer.h"
#include "TrajectoriesDrawer.h"


class TrajectoriesDialog : public QDialog
{
	Q_OBJECT;
public:
	TrajectoriesDialog(QWidget* parent, TrajectoryDrawerPtr trajectoryDrawer);

public:
	void setMarkers(MarkerCollectionConstPtr markers);

private slots:
	void onTreeItemClicked(QTreeWidgetItem* item, int column);
	void colorClicked();
	void visibilityChanged(bool visible);
	void widthChanged(double width);

private:
	QColor transformColor(const osg::Vec4& color) const;
	osg::Vec4 transformColor(const QColor& color) const;
	QTreeWidgetItem* getItemWhichContains(QObject* object) const;
	void setButtonColor(QPushButton* button, const QColor& color);

private:
	TrajectoryDrawerPtr trajectories;
	QTableView* table;
	QTreeWidget* tree;
};


#endif
