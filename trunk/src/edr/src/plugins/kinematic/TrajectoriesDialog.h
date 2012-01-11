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
#include "TrajectoriesDrawer.h"
#include "ui_TrajectoriesDialog.h"


class TrajectoriesDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
	TrajectoriesDialog(QWidget* parent);

public:
	void setMarkers(TrajectoryDrawerPtr drawer, const QString& rootName);

private slots:
	void colorClicked();
	void visibilityChanged(bool visible);
	void widthChanged(double width);
	void startTimeChanged(double time);
	void endTimeChanged(double time);
	void treeItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	void startSliderChanged(int value);
	void endSliderChanged(int value);

private:
	QColor transformColor(const osg::Vec4& color) const;
	osg::Vec4 transformColor(const QColor& color) const;
	QTreeWidgetItem* getItemWhichContains(QObject* object) const;
	void setButtonColor(QPushButton* button, const QColor& color);
	void blockAllSignals(bool val);

private:
	TrajectoryDrawerPtr currentTrajectories;
    std::map<QTreeWidgetItem*, TrajectoryDrawerPtr> item2Trajectories;
};


#endif
